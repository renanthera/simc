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

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

struct event_t;
struct expr_t;
struct option_t;
// struct player_t;
struct sim_t;

template <typename T>
struct interval_t
{
  T min;
  T max;
  T mean;
  T stddev;
  interval_t();
  void sc_format_to( const interval_t& interval, fmt::format_context::iterator out );
};

enum actor_filter_e
{
  AF_NONE       = 1 << 0,
  AF_ACTOR_TYPE = 1 << 1,  // player_e enemy, player, (player) pet
  AF_ROLE       = 1 << 2,  // role_e dps, tank, heal
  AF_EXPRN      = 1 << 3,  // expr_t expression
  AF_MAX        = 1 << 4,
};

struct actor_filter_t
{
  std::string input;
  actor_filter_e filter_type;
  union
  {
    player_e actor;
    role_e role;
  } filter;
  actor_filter_t();
  void sc_format_to( const actor_filter_t& actor_filter, fmt::format_context::iterator out );
};

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
