#include "sc_monk.hpp"

// Each action or effect is contained within a struct `action_name_t`.
// If it has multiple component action or effects which are not used externally,
// they should be placed within `action_name_t`.
// All essential logic beyond triggering and execution must be self contained within
// the action or effect.

// Ordering is important, as an action used in another action must be defined first.
// Make sure this file remains clang-format'ed.

namespace monk
{
template <class Base>
struct monk_action_t : public Base, public parse_buff_effects_t<monk_td_t>
{
  using base_t = monk_action_t<Base>;

  monk_action_t( std::string_view name, monk_t* player, const spell_data_t* spell_data )
    : Base( name, player, spell_data ), parse_buff_effects_t( this )
  {
    // only track cd waste for abilities with non-zero cds.
    base_t::track_cd_waste = base_t::data().cooldown() > 0_s || base_t::data().charge_cooldown() > 0_s;

    if ( base_t::data().ok() )
    {
      apply_buff_effects();
      apply_debuff_effects();
    }
  }

  void apply_buff_effects()
  {
  }
  void apply_debuff_effects()
  {
  }

  monk_t* player()
  {
    return debug_cast<monk_t*>( Base::player );
  }
  const monk_t* player() const
  {
    return debug_cast<monk_t*>( Base::player );
  }
};

struct monk_attack_t : public monk_action_t<melee_attack_t>
{
  monk_attack_t( std::string_view name, monk_t* player, const spell_data_t* spell = spell_data_t::nil() )
    : base_t( name, player, spell )
  {
    special    = true;
    may_glance = false;
  }
};

struct monk_heal_t : public monk_action_t<heal_t>
{
  monk_heal_t( std::string_view name, monk_t* player, const spell_data_t* spell = spell_data_t::nil() )
    : base_t( name, player, spell )
  {
    ap_type = attack_power_type::WEAPON_MAINHAND;  // TODO: NECESSARY?
    harmful = false;
    special = true;
  }
};

struct monk_absorb_t : public monk_action_t<absorb_t>
{
  monk_absorb_t( std::string_view name, monk_t* player, const spell_data_t* spell = spell_data_t::nil() )
    : base_t( name, player, spell )
  {
    ap_type = attack_power_type::WEAPON_MAINHAND;  // TODO: NECESSARY?
    harmful = false;
    special = true;
  }
};

// Ability: Blackout Kick
struct blackout_kick_t : public monk_attack_t
{
  blackout_kick_t( monk_t* player, std::string_view options_str )
    : monk_attack_t( "blackout_kick", player, player->ability.general.blackout_kick )
  {
    parse_options( options_str );
  }
};

// Ability: Auto Attack
struct auto_attack_t : public monk_attack_t
{
  struct melee_t : public monk_attack_t
  {
    slot_e weapon_slot;

    melee_t( std::string_view name, monk_t* player, slot_e weapon_slot )
      : monk_attack_t( name, player ), weapon_slot( weapon_slot )
    {
      background                = true;
      repeating                 = true;
      may_glance                = true;
      may_crit                  = true;
      special                   = false;
      trigger_gcd               = 0_s;
      school                    = SCHOOL_PHYSICAL;
      weapon_multiplier         = 1.0;
      allow_class_ability_procs = true;
      not_a_proc                = true;
      id                        = 1;
      // Weapon is a member of action_t that must be set for melees to function,
      // thus cannot be set via initializer list.
      weapon            = &( weapon_slot == SLOT_MAIN_HAND ? player->main_hand_weapon : player->off_hand_weapon );
      base_execute_time = weapon->swing_time;

      // Mistweaver melees never miss, but can be parried.
      // Brewmaster melees can miss if dual-wielding, but not be parried.
      // Windwalker melees can miss if dual-wielding, (TODO: COMPLETE VERIFICATION)
      if ( player->main_hand_weapon.group() == WEAPON_1H && player->specialization() != MONK_MISTWEAVER )
        base_hit -= 0.19;
    }
  };

  melee_t* main_hand;
  melee_t* off_hand;

  auto_attack_t( monk_t* player, std::string_view options_str ) : monk_attack_t( "auto_attack", player )
  {
    parse_options( options_str );
    id                    = 1;  // required for the action to execute
    trigger_gcd           = 0_s;
    special               = false;
    ignore_false_positive = true;  // disables "player skill" parameters

    main_hand = new melee_t( "melee_main_hand", player, SLOT_MAIN_HAND );
    add_child( main_hand );
    if ( player->off_hand_weapon.type != WEAPON_NONE && player->dual_wield() )
    {
      off_hand = new melee_t( "melee_off_hand", player, SLOT_OFF_HAND );
      add_child( off_hand );
    }
  }

  bool ready() override
  {
    if ( player()->current.distance_to_move > 5 )
      return false;
    return ( main_hand->execute_event == nullptr || off_hand->execute_event == nullptr );
  }

  void execute() override
  {
    if ( main_hand->execute_event == nullptr && main_hand )
      main_hand->schedule_execute();
    if ( off_hand->execute_event == nullptr && off_hand )
      off_hand->schedule_execute();
  }
};

// Action: Chi Surge
struct chi_surge_t : public monk_attack_t
{
  chi_surge_t( monk_t* player )
    : monk_attack_t( "chi_surge", player, player->talent.brewmaster.chi_surge->effectN( 1 ).trigger() )
  {
    dual             = true;
    background       = true;
    target           = player;
    aoe              = -1;
    split_aoe_damage = true;
  }

  double composite_persistent_multiplier( const action_state_t* state ) const override
  {
    return monk_attack_t::composite_persistent_multiplier( state );
    return monk_attack_t::composite_persistent_multiplier( state ) / state->n_targets;
  }

  void impact( action_state_t* state ) override
  {
    monk_attack_t::impact( state );
    double cdr = player()->talent.brewmaster.chi_surge->effectN( 1 ).base_value();
    player()->cooldown.weapons_of_order->adjust( timespan_t::from_seconds( -1.0 * cdr ) );
  }
};

// Ability: Weapons of Order
struct weapons_of_order_t : public monk_attack_t
{
  weapons_of_order_t( monk_t* player, std::string_view options_str )
    : monk_attack_t( "weapons_of_order", player, player->talent.brewmaster.weapons_of_order )
  {
    parse_options( options_str );
    harmful = player->talent.brewmaster.call_to_arms.enabled();
  }

  void execute() override
  {
    monk_attack_t::execute();
    player()->action.chi_surge->execute();
  }
};

action_t* monk_t::create_action( std::string_view name, std::string_view options_str )
{
  // Baseline Abilities
  if ( name == "blackout_kick" )
    return new blackout_kick_t( this, options_str );
  if ( name == "auto_attack" )
    return new auto_attack_t( this, options_str );
  if ( name == "weapons_of_order" )
    return new weapons_of_order_t( this, options_str );

  return player_t::create_action( name, options_str );
}
}  // namespace monk
