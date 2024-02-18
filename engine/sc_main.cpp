// ==========================================================================
// Dedmonwakeen's Raid DPS/TPS Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include "class_modules/class_module.hpp"
#include "dbc/dbc.hpp"
#include "dbc/spell_query/spell_data_expr.hpp"
#include "interfaces/bcp_api.hpp"
#include "interfaces/sc_http.hpp"
#include "lib/fmt/core.h"
#include "player/player.hpp"
#include "player/unique_gear.hpp"
#include "report/reports.hpp"
#include "sim/plot.hpp"
#include "sim/reforge_plot.hpp"
#include "sim/profileset.hpp"
#include "sim/sim.hpp"
#include "sim/scale_factor_control.hpp"
#include "sim/sim_control.hpp"
#include "util/git_info.hpp"
#include "util/io.hpp"

#include <locale>

#include "util/rng.hpp"
#include "util/timespan.hpp"
#include <iostream>
#include <random>

#ifdef SC_SIGACTION
#include <csignal>
#include <utility>
#endif

namespace { // anonymous namespace ==========================================

#ifdef SC_SIGACTION
// POSIX-only signal handler ================================================

struct sim_signal_handler_t
{
  static sim_t* global_sim;

  static void report( int signal )
  {
    const char* name = strsignal( signal );
    fmt::print( stderr, "sim_signal_handler: {}!", name );
    const sim_t* crashing_child = nullptr;

#ifndef SC_NO_THREADING
    if ( signal == SIGSEGV )
    {
      for ( auto child : global_sim -> children )
      {
        if ( std::this_thread::get_id() == child ->  thread_id() )
        {
          crashing_child = child;
          break;
        }
      }
    }
#endif

    if ( crashing_child )
    {
      fmt::print(stderr, " Thread={} Iteration={} Seed={} ({}) TargetHealth={}\n",
          crashing_child -> thread_index, crashing_child -> current_iteration, crashing_child -> seed,
          ( crashing_child -> seed + crashing_child -> thread_index ),
          crashing_child -> target -> resources.initial[ RESOURCE_HEALTH ]);
    }
    else
    {
      fmt::print(stderr, " Iteration={} Seed={} TargetHealth={}\n",
          global_sim -> current_iteration, global_sim -> seed,
          global_sim -> target -> resources.initial[ RESOURCE_HEALTH ]);
    }

    auto profileset = global_sim -> profilesets->current_profileset_name();
    if ( ! profileset.empty() )
    {
      fmt::print( stderr, " ProfileSet={}", profileset );
    }

    fmt::print( stderr, "\n" );
    std::fflush( stderr );
  }

  static void sigint( int signal )
  {
    if ( global_sim )
    {
      report( signal );
      if( global_sim -> scaling -> calculate_scale_factors || global_sim -> reforge_plot -> current_reforge_sim || global_sim -> plot -> current_plot_stat != STAT_NONE )
      {
        global_sim -> cancel();
      }
      else if ( global_sim -> single_actor_batch )
      {
        global_sim -> cancel();
      }
      else if ( ! global_sim -> profileset_map.empty() )
      {
        global_sim -> cancel();
      }
      else
      {
        global_sim -> interrupt();
      }
    }
  }

  static void sigsegv( int signal )
  {
    if ( global_sim )
    {
      report( signal );
    }
    exit( signal );
  }

  sim_signal_handler_t()
  {
    assert ( ! global_sim );

    struct sigaction sa;
    sigemptyset( &sa.sa_mask );
    sa.sa_flags = 0;

    sa.sa_handler = sigsegv;
    sigaction( SIGSEGV, &sa, nullptr );

    sa.sa_handler = sigint;
    sigaction( SIGINT,  &sa, nullptr );
  }

  ~sim_signal_handler_t()
  { global_sim = nullptr; }
};
#else
struct sim_signal_handler_t
{
  static sim_t* global_sim;
};
#endif

sim_t* sim_signal_handler_t::global_sim = nullptr;

[[maybe_unused]] sim_signal_handler_t handler;

// need_to_save_profiles ====================================================

bool need_to_save_profiles( sim_t* sim )
{
  if ( sim -> save_profiles ) { return true;
}

  for ( auto& player : sim -> player_list )
  {
    if ( ! player -> report_information.save_str.empty() ) {
      return true;
}
  }

  return false;
}

/* Obtain a platform specific place to store the http cache file
 */
std::string get_cache_directory()
{
  std::string s = ".";

  const char* env; // store desired environemental variable in here. getenv returns a null pointer if specified
  // environemental variable cannot be found.
#if defined(__linux__) || defined(__APPLE__)
  env = getenv( "XDG_CACHE_HOME" );
  if ( ! env )
  {
    env = getenv( "HOME" );
    if ( env ) {
      s = std::string( env ) + "/.cache";
    } else {
      s = "/tmp"; // back out
}
  }
  else {
    s = std::string( env );
}
#endif
#ifdef _WIN32
#pragma warning( push )
  // Disable security warning
#pragma warning( disable : 4996 )
  env = std::getenv( "TMP" );
  if ( !env )
  {
    env = std::getenv( "TEMP" );
    if ( ! env )
    {
      env = std::getenv( "HOME" );
    }
  }
  s = std::string( env );
#pragma warning( pop )
#endif

  return s;
}

// RAII-wrapper for http cache load / save
struct cache_initializer_t {
  cache_initializer_t( std::string  fn ) :
    _file_name(std::move( fn ))
  { http::cache_load( _file_name ); }
  ~cache_initializer_t()
  { http::cache_save( _file_name ); }
private:
  std::string _file_name;
};

struct apitoken_initializer_t
{
  apitoken_initializer_t()
  { bcp_api::token_load(); }

  ~apitoken_initializer_t()
  { bcp_api::token_save(); }
};

struct special_effect_initializer_t
{
  special_effect_initializer_t()
  {
    unique_gear::register_special_effects();
    unique_gear::sort_special_effects();
  }

  ~special_effect_initializer_t()
  { unique_gear::unregister_special_effects(); }
};

void print_version_info( const dbc_t& dbc )
{
  fmt::print( "{}", util::version_info_str( &dbc ) );
}

void print_build_info ( const dbc_t& dbc )
{
  fmt::print( " ({})", util::build_info_str( &dbc ) );
}

} // anonymous namespace ====================================================

// sim_t::main ==============================================================

int sim_t::main( const std::vector<std::string>& args )
{
  try
  {
    cache_initializer_t cache_init( get_cache_directory() + "/simc_cache.dat" );
    apitoken_initializer_t apitoken_init;
    dbc::init();
    module_t::init();
    unique_gear::register_hotfixes();

    special_effect_initializer_t special_effect_init;

    print_version_info( *dbc );

    sim_control_t control;

    try
    {
      control.options.parse_args( args );
    }
    catch ( const std::exception& )
    {
      fmt::print( "\n" );
      std::throw_with_nested( std::invalid_argument( "Incorrect option format" ) );
    }

    // Hotfixes are applies right before the sim context (control) is created, and simulator setup begins
    hotfix::apply();

    try
    {
      setup( &control );
    }
    catch ( const std::exception& )
    {
      fmt::print( "\n" );
      std::throw_with_nested( std::runtime_error( "Setup failure" ) );
    }

    if ( display_build > 0 )
      print_build_info( *dbc );

    // newline as print_version_info does not include it
    fmt::print( "\n" );

    if ( display_build > 1 )
      return 0;

    if ( display_hotfixes )
    {
      fmt::print( "{}", hotfix::to_str( dbc->ptr ) );
      return 0;
    }

    if ( display_bonus_ids )
    {
      fmt::print( "{}", dbc::bonus_ids_str( *dbc ) );
      return 0;
    }

    if ( canceled )
    {
      return 1;
    }

    if ( spell_query )
    {
      try
      {
        spell_query->evaluate();
        print_spell_query();
      }
      catch ( const std::exception& )
      {
        std::throw_with_nested( std::runtime_error( "Spell Query Error" ) );
      }
    }
    else if ( need_to_save_profiles( this ) )
    {
      try
      {
        init();
        fmt::print( "\nGenerating profiles... \n" );
        report::print_profiles( this );
      }
      catch ( const std::exception& )
      {
        std::throw_with_nested( std::runtime_error( "Generating profiles" ) );
      }
    }
    else
    {
      fmt::print(
          "\nSimulating... ( iterations={}, threads={}, target_error={:.3f}, max_time={:.0f}, "
          "vary_combat_length={:0.2f}, optimal_raid={}, fight_style={} )\n\n",
          iterations, threads, target_error, max_time.total_seconds(), vary_combat_length, optimal_raid, fight_style );

      progress_bar.set_base( "Baseline" );
      if ( execute() )
      {
        scaling->analyze();
        plot->analyze();
        reforge_plot->analyze();

        if ( canceled == 0 && !profilesets->iterate( this ) )
        {
          canceled = true;
        }
        else
        {
          report::print_suite( this );
        }
      }
      else
      {
        fmt::print( "Simulation was canceled.\n" );
        canceled = true;
      }
    }

    fmt::print( "\n" );

    return canceled;
  }
  catch ( const std::nested_exception& e )
  {
    // Only catch exception we have already re-thrown in init functions.
    fmt::print( stderr, "Error: " );
    util::print_chained_exception( e.nested_ptr(), stderr );
    fmt::print( stderr, "\n" );
    return 1;
  }
}

// ==========================================================================
// MAIN
// ==========================================================================

int main( int argc, char** argv )
{
  std::random_device rd;
  rng::basic_rng_t<rng::xoshiro256plus_t> rng{};
  uint count = 50000;

  rng.seed( 123456789 );

  double roll_d = 0.5;

  double min_d = 0.0;
  int min_i = 0;
  uint min_u = 0;
  timespan_t min_t = 0_s;

  double max_d = 2.0;
  int max_i = 2;
  uint max_u = 2;
  timespan_t max_t = 2_s;

  double mean_d = 0.0;
  uint mean_u = 0;
  timespan_t mean_t = 0_s;

  double stddev_d = 1.0;
  uint stddev_u = 1;
  timespan_t stddev_t = 1_s;

  std::cout << "double real()" << std::endl;
  for (uint i = 0; i < count; i++)
    std::cout << rng.real() << std::endl;

  std::cout << "bool roll()" << std::endl;
  for (uint i = 0; i < count; i++)
    std::cout << rng.roll( roll_d ) << std::endl;

  std::cout << "double range(double min, double max)" << std::endl;
  for (uint i = 0; i < count; i++)
    std::cout << rng.range(min_d, max_d) << std::endl;

  std::cout << "T range(T min, T max); T=int" << std::endl;
  for (uint i = 0; i < count; i++)
    std::cout << rng.range(min_i, max_i) << std::endl;

  std::cout << "T range(T min, T max); T=uint" << std::endl;
  for (uint i = 0; i < count; i++)
    std::cout << rng.range(min_u, max_u) << std::endl;

  std::cout << "timespan_t range(min, max)" << std::endl;
  for (uint i = 0; i < count; i++)
    std::cout << rng.range(min_t, max_t).total_millis() << std::endl;

  std::cout << "double range(double max)" << std::endl;
  for (uint i = 0; i < count; i++)
    std::cout << rng.range(max_d) << std::endl;

  std::cout << "T range(T max); T=int" << std::endl;
  for (uint i = 0; i < count; i++)
    std::cout << rng.range(max_i) << std::endl;

  std::cout << "T range(T max); T=uint" << std::endl;
  for (uint i = 0; i < count; i++)
    std::cout << rng.range(max_u) << std::endl;

  std::cout << "double gauss(...)" << std::endl;
  for (uint i = 0; i < count; i++)
    std::cout << rng.gauss(mean_d, stddev_d) << std::endl;

  std::cout << "double gauss_ab(...)" << std::endl;
  for (uint i = 0; i < count; i++)
    std::cout << rng.gauss_ab(mean_d, stddev_d, min_d, max_d) << std::endl;

  std::cout << "double gauss_a(...)" << std::endl;
  for (uint i = 0; i < count; i++)
    std::cout << rng.gauss_a(mean_d, stddev_d, min_d) << std::endl;

  std::cout << "double gauss_b(...)" << std::endl;
  for (uint i = 0; i < count; i++)
    std::cout << rng.gauss_b(mean_d, stddev_d, max_d) << std::endl;

  std::cout << "T gauss(...); T=uint" << std::endl;
  for (uint i = 0; i < count; i++)
    std::cout << rng.gauss(mean_u, stddev_u) << std::endl;

  std::cout << "timespan_t gauss(...)" << std::endl;
  for (uint i = 0; i < count; i++)
    std::cout << rng.gauss(mean_t, stddev_t).total_millis() << std::endl;

  std::cout << "double exponential(...)" << std::endl;
  for (uint i = 0; i < count; i++)
    std::cout << rng.exponential(stddev_d) << std::endl;

  std::cout << "timespan_t exponential(...)" << std::endl;
  for (uint i = 0; i < count; i++)
    std::cout << rng.exponential(stddev_t).total_millis() << std::endl;

  std::cout << "double exgauss(...)" << std::endl;
  for (uint i = 0; i < count; i++)
    std::cout << rng.exgauss(mean_d, stddev_d, stddev_d) << std::endl;

  std::cout << "T exgauss(...); T=uint" << std::endl;
  for (uint i = 0; i < count; i++)
    std::cout << rng.exgauss(mean_u, stddev_u, stddev_u) << std::endl;

  std::cout << "timespan_t exgauss(...)" << std::endl;
  for (uint i = 0; i < count; i++)
    std::cout << rng.exgauss(mean_t, stddev_t, stddev_t).total_millis() << std::endl;
  // std::locale::global();

  // sim_t sim;
  // sim_signal_handler_t::global_sim = &sim;

  // return sim.main( io::utf8_args( argc, argv ) );
}
