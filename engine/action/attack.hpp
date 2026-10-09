// ==========================================================================
// Dedmonwakeen's Raid DPS/TPS Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#pragma once

#include "config.hpp"

#include "action.hpp"
#include "player/player.hpp"
#include "item/item.hpp"

// WDPS -> Attack Power Coefficient used for Forever/Classic Attack Power calculations
constexpr double WEAPON_POWER_COEFFICIENT = 14;

struct attack_t : public action_t
{
  double base_attack_expertise;

  attack_t( util::string_view token, player_t* p );
  attack_t( util::string_view token, player_t* p, const spell_data_t* s );

  // Attack Overrides
  double execute_time_pct_multiplier() const override;
  void execute() override;
  result_e calculate_result( action_state_t* ) const override;

  result_amount_type amount_type( const action_state_t* /* state */, bool /* periodic */ = false ) const override;
  result_amount_type report_amount_type( const action_state_t* /* state */ ) const override;

  double miss_chance( double hit, player_t* t ) const override;
  double dodge_chance( double /* expertise */, player_t* t ) const override;

  double bonus_da( const action_state_t* ) const override;
  double action_multiplier() const override;

  double composite_target_multiplier( player_t* ) const override;
  double composite_hit() const override;
  double composite_crit_chance() const override;
  double composite_crit_chance_multiplier() const override;
  double composite_haste() const override;
  double recharge_multiplier( const cooldown_t& cd ) const override;

  virtual double composite_expertise() const;
  virtual void reschedule_auto_attack( double old_swing_haste );

  void reset() override;

private:
  /// attack table generator with caching
  struct attack_table_t
  {
    std::array<double, RESULT_MAX> chances;
    std::array<result_e, RESULT_MAX> results;
    int num_results;
    double attack_table_sum;  // Used to check whether we can use cached values or not.

    attack_table_t()
    {
      reset();
    }

    void reset()
    {
      attack_table_sum = std::numeric_limits<double>::min();
    }

    void build_table( double miss_chance, double dodge_chance, double parry_chance, double glance_chance,
                      double crit_chance, sim_t* );
  };

  mutable attack_table_t attack_table;
};

// Melee Attack ===================================================================

struct melee_attack_t : public attack_t
{
  melee_attack_t( util::string_view token, player_t* p );
  melee_attack_t( util::string_view token, player_t* p, const spell_data_t* s );

  // Melee Attack Overrides
  void init() override;
  double parry_chance( double /* expertise */, player_t* t ) const override;
  double glance_chance( int delta_level ) const override;

  proc_types proc_type() const override;
};

// Ranged Attack ===================================================================

struct ranged_attack_t : public attack_t
{
  ranged_attack_t( util::string_view token, player_t* p );
  ranged_attack_t( util::string_view token, player_t* p, const spell_data_t* s );

  // Ranged Attack Overrides
  double composite_target_multiplier( player_t* ) const override;
  void schedule_execute( action_state_t* execute_state = nullptr ) override;

  proc_types proc_type() const override;
};

struct auto_attack_t : public action_t
{
  std::map<slot_e, action_t*> attacks;

  // static bool is_ranged(weapon_t& w)
  // {
  //   switch ( w->)
  // };

  // virtual action_t* create_melee_attack(weapon_t& w, player_t* p)
  // {
  //   return new melee_attack_t()
  // };

  auto_attack_t( std::string_view options_str, player_t* p )
    : action_t( ACTION_OTHER, "auto_attack", p )
  {
    parse_options( options_str );

    for (const item_t& item : p->items )
    {
      switch ( item.slot )
        {
        case SLOT_MAIN_HAND:
        case SLOT_OFF_HAND:
        case SLOT_RANGED:
          // foo
        default:
          continue;
        }
    }
  }
};
