#include "sc_monk_abilities.hpp"

#include "sc_monk.hpp"

namespace monk
{
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
      weapon            = &( weapon_slot == SLOT_MAIN_HAND ? player->main_hand_weapon : player->off_hand_weapon );
      base_execute_time = weapon->swing_time;

      if ( player->main_hand_weapon.group() == WEAPON_1H && player->specialization() != MONK_MISTWEAVER )
        base_hit -= 0.19;
    }
  };

  melee_t* main_hand;
  melee_t* off_hand;

  auto_attack_t( monk_t* player, std::string_view options_str ) : monk_attack_t( "auto_attack", player )
  {
    parse_options( options_str );
    id                    = 1;
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
}  // namespace monk
