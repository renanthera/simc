// ==========================================================================
// Dedmonwakeen's Raid DPS/TPS Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#pragma once

#include "config.hpp"

#include "sc_enums.hpp"
#include "util/timespan.hpp"
#include "util/generic.hpp"
#include "util/string_view.hpp"
#include "util/format.hpp"
#include "util/io.hpp"
#include "report/report_helper.hpp"
#include "util/util.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <regex>

struct event_t;
struct expr_t;
struct option_t;
struct player_t;
struct sim_t;

template <typename T>
struct interval_t
{
  // Previous behaviour:
  // - values are normally distributed (ish)
  // - mean is centrally located between max and min
  // - all possible values are contained within [ mean - 6 * stddev, mean + 6 * stddev]

  // if not provided, min = mean * 0.5
  // if not provided, max = mean * 1.5
  // if not provided, stddev = 0
  // if not provided, mean = 0

  // some values are considered not initializable:
  // - ( mean - 6 * stddev <= 0 ) @ adds_raid_event_t::adds_raid_event_t()
  // - ( mean - stddev <= 0 ) @ raid_event_t::init(...)
  // - ( mean <= 0 ) @ raid_event_t::init(...)

  // New behaviour:
  // - if value is defined, use value
  // - mean: mandatory
  // - stddev: gauss, otherwise uniform
  // - min: -> gauss_b or gauss
  // - max: -> gauss_a or gauss
  sim_t* sim;
  T mean;
  T stddev;
  T min;
  T max;
  interval_t( sim_t* sim, T mean, T stddev, T min, T max )
    : sim( sim ),
      mean( mean ),
      stddev( stddev ),
      min( min ),
      max( max )
  {
    // if ( max < min && max != 0.0 )
    //   throw std::invalid_argument( fmt::format( "Invalid arguments: Max ({}) less than Min ({})", max, min ) );
    // if ( mean < min && mean != -1 )
    //   throw std::invalid_argument( fmt::format( "Invalid arguments: Mean ({}) less than Min ({})", mean, min ) );
    // if ( mean > max && max != -1 )
    //   throw std::invalid_argument( fmt::format( "Invalid arguments: Mean ({}) greater than than Max ({})", mean, max ) );
    // if ( min < stddev * 6 )
    //   throw std::invalid_argument( fmt::format( "Invalid arguments: Stddev ({}) too large.", stddev ) );
  }

  T value()
  {
    if ( stddev && min && max )
      return sim->rng().gauss_ab( mean, stddev, min, max );
    if ( stddev && min )
      return sim->rng().gauss_a( mean, stddev, min );
    if ( stddev && max )
      return sim->rng().gauss_b( mean, stddev, max );
    if ( stddev )
      return sim->rng().gauss( mean, stddev );
    return sim->rng().range( min, max );
  }
};

struct actor_filter_t
{
  std::string filter_input;
  std::string filter_type;
  actor_filter_t( std::string filter_input, std::string filter_type )
    : filter_input( filter_input ),
      filter_type( filter_type )
  {};

  virtual bool filter_actor( player_t* p );

  void apply_filter( std::vector<player_t*>& actor_list )
  {
    actor_list.erase( std::remove_if( actor_list.begin(), actor_list.end(),
                                       [ this ]( player_t* p ) { return filter_actor( p ); } ) );
  }

  void sc_format_to( const actor_filter_t& actor_filter, fmt::format_context::iterator out )
  {
    fmt::format_to( out, "Filters {}: {}", actor_filter.filter_type, actor_filter.filter_input );
  }
};

struct af_actor_type_t : actor_filter_t
{
  player_e filter;

  af_actor_type_t( std::string filter_input )
    : actor_filter_t( filter_input, "actor type" ),
      filter( util::parse_player_type( filter_input ) )
  {};

  bool filter_actor( player_t* p )
  {
    return p->type == filter;
  }
};

struct af_role_type_t : actor_filter_t
{
  role_e filter;

  af_role_type_t( std::string filter_input )
    : actor_filter_t( filter_input, "actor role" ),
      filter( util::parse_role_type( filter_input ) )
  {};

  bool filter_actor( player_t* p )
  {
    return p->primary_role() == filter;
  }
};

struct af_name_string_t : actor_filter_t
{
  af_name_string_t( std::string filter_input )
    : actor_filter_t( filter_input, "actor name (string)" )
  {};

  bool filter_actor( player_t* p )
  {
    return p->name() == filter_input;
  }
};

struct af_name_regex_t : actor_filter_t
{
  std::regex filter;

  af_name_regex_t( std::string filter_input )
    : actor_filter_t( filter_input, "actor name (regex)" ),
      filter( filter_input )
  {};

  bool filter_actor( player_t* p )
  {
    return std::regex_search( p->name(), filter );
  }
};

// struct af_expression_t : actor_filter_t
// {};

struct raid_event_t : private noncopyable
{
public:
  sim_t* sim;
  std::string type;
  int64_t num_starts;
  timespan_t saved_duration;
  std::unordered_map<size_t, std::unique_ptr<expr_t>> player_expressions;

  struct options_t
  {
    std::string name;
    interval_t<timespan_t> time_trigger;
    interval_t<double> percent_trigger;
    interval_t<timespan_t> cooldown;
    interval_t<timespan_t> duration;
    std::vector<actor_filter_t> target_filters;
    std::vector<actor_filter_t> source_filters;
    std::vector<player_t*> target_actors;
    std::vector<player_t*> source_actors;
    bool stop; // stop immediately at any `last` trigger
    double execute_chance;  // Chance for individual targets to be affected by raid event
    // interval_t<double> distance;
    // int pull_index;
    options_t();
    void sc_format_to( const options_t& options, fmt::format_context::iterator out );
  } option;
  std::vector<std::unique_ptr<option_t>> options;

  raid_event_t( sim_t*, util::string_view type );

  virtual ~raid_event_t() = default;

  virtual bool filter_player( const player_t* );

  void add_option( std::unique_ptr<option_t> new_option )
  {
    options.insert( options.begin(), std::move( new_option ) );
  }
  timespan_t cooldown_time();
  virtual timespan_t duration_time();
  timespan_t next_time() const;
  timespan_t until_next() const;
  virtual timespan_t remains() const;
  bool up() const;
  // double distance()
  // {
  //   return distance_max;
  // }
  // double max_distance()
  // {
  //   return distance_min;
  // }
  // double min_distance()
  // {
  //   return distance_max;
  // }
  void schedule();
  void deactivate( util::string_view reason );
  virtual void reset();
  virtual void combat_begin();
  void parse_options( util::string_view options_str );
  static std::unique_ptr<raid_event_t> create( sim_t* sim, util::string_view name, util::string_view options_str );
  static void init( sim_t* );
  static void reset( sim_t* );
  static void combat_begin( sim_t* );
  static void combat_end( sim_t* )
  {
  }
  static void merge( sim_t* sim, sim_t* other );
  static void analyze( sim_t* sim );
  static void report( sim_t* sim, report::sc_html_stream& os );
  static double evaluate_raid_event_expression( sim_t* s, util::string_view type, util::string_view filter,
                                                bool test_filter, bool* is_constant );

private:
  virtual void _start()  = 0;
  virtual void _finish() = 0;
  void activate( util::string_view reason );
  void start();
  void finish();

  bool is_up;
  enum class activation_status_e
  {
    // three different states so we can detect when raid event is deactivated before it is activated.
    not_yet_activated,
    activated,
    deactivated
  } activation_status;
  event_t* cooldown_event;
  event_t* duration_event;
  event_t* start_event;
  event_t* end_event;

  friend void sc_format_to( const raid_event_t&, fmt::format_context::iterator );
};
