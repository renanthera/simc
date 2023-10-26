// ==========================================================================
// Dedmonwakeen's DPS-DPM Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#pragma once

#include "action/action_callback.hpp"
#include "action/parse_buff_effects.hpp"
#include "player/pet_spawner.hpp"
#include "sc_enums.hpp"
#include "sc_monk_abilities.hpp"

#include "simulationcraft.hpp"

namespace monk
{
struct monk_t : public player_t
{
  // melee_t* main_hand_temp;
  // melee_t* off_hand_temp;

  // Member definitions
  // Augmentations to reported charts and data.
  struct sample_data_t
  {
  } sample_data;

  // Abilities
  struct ability_t
  {
    struct shared_t
    {
    } shared;
    struct general_t
    {
      const spell_data_t* blackout_kick;
    } general;

    struct brewmaster_t
    {
    } brewmaster;

    struct windwalker_t
    {
    } windwalker;

    struct mistweaver_t
    {
    } mistweaver;
  } ability;

  // Passives
  struct passive_t
  {
    struct shared_t
    {
    } shared;

    struct general_t
    {
      const spell_data_t* class_aura;
      const spell_data_t* mystic_touch;  // TODO: IMPL
      const spell_data_t* leather_specialization;
    } general;

    struct brewmaster_t
    {
      const spell_data_t* spec_aura;
      const spell_data_t* leather_specialization;
      const spell_data_t* celestial_fortune;  // TODO: IMPL
    } brewmaster;

    struct windwalker_t
    {
      const spell_data_t* leather_specialization;
    } windwalker;

    struct mistweaver_t
    {
      const spell_data_t* leather_specialization;
    } mistweaver;
  } passive;

  // Talents
  struct talent_t
  {
    struct general_t
    {
      player_talent_t soothing_mist;               // TODO: IMPL // ROW 1
      player_talent_t rising_sun_kick;             // TODO: IMPL
      player_talent_t tigers_lust;                 // TODO: IMPL
      player_talent_t improved_roll;               // TODO: IMPL // ROW 2
      player_talent_t calming_presence;            // TODO: IMPL
      player_talent_t disable;                     // TODO: IMPL
      player_talent_t tiger_tail_sweep;            // TODO: IMPL // ROW 3
      player_talent_t vigorous_expulsion;          // TODO: IMPL
      player_talent_t improved_vivify;             // TODO: IMPL
      player_talent_t detox;                       // TODO: IMPL
      player_talent_t improved_detox;              // TODO: IMPL
      player_talent_t paralysis;                   // TODO: IMPL
      player_talent_t grace_of_the_crane;          // TODO: IMPL // ROW 4
      player_talent_t vivacious_vivification;      // TODO: IMPL
      player_talent_t ferocity_of_xuen;            // TODO: IMPL
      player_talent_t improved_paralysis;          // TODO: IMPL
      player_talent_t elusive_mists;               // TODO: IMPL // ROW 5
      player_talent_t transcendence;               // TODO: IMPL
      player_talent_t spear_hand_strike;           // TODO: IMPL
      player_talent_t fortifying_brew;             // TODO: IMPL
      player_talent_t chi_wave;                    // TODO: IMPL // ROW 6
      player_talent_t chi_burst;                   // TODO: IMPL
      player_talent_t hasty_provocation;           // TODO: IMPL
      player_talent_t ring_of_peace;               // TODO: IMPL
      player_talent_t song_of_chiji;               // TODO: IMPL
      player_talent_t fast_feet;                   // TODO: IMPL
      player_talent_t celerity;                    // TODO: IMPL
      player_talent_t chi_torpedo;                 // TODO: IMPL
      player_talent_t ironshell_brew;              // TODO: IMPL
      player_talent_t expeditious_fortification;   // TODO: IMPL
      player_talent_t profound_rebuttal;           // TODO: IMPL // ROW 7
      player_talent_t yulons_grace;                // TODO: IMPL
      player_talent_t diffuse_magic;               // TODO: IMPL
      player_talent_t eye_of_the_tiger;            // TODO: IMPL
      player_talent_t dance_of_the_wind;           // TODO: IMPL
      player_talent_t dampen_harm;                 // TODO: IMPL
      player_talent_t improved_touch_of_death;     // TODO: IMPL
      player_talent_t strength_of_spirit;          // TODO: IMPL
      player_talent_t close_to_heart;              // TODO: IMPL // ROW 8
      player_talent_t escape_from_reality;         // TODO: IMPL
      player_talent_t windwalking;                 // TODO: IMPL
      player_talent_t fatal_touch;                 // TODO: IMPL
      player_talent_t generous_pour;               // TODO: IMPL
      player_talent_t save_them_all;               // TODO: IMPL // ROW 9
      player_talent_t resonant_fists;              // TODO: IMPL
      player_talent_t bounce_back;                 // TODO: IMPL
      player_talent_t summon_jade_serpent_statue;  // TODO: IMPL // ROW 10
      player_talent_t summon_white_tiger_statue;   // TODO: IMPL
      player_talent_t summon_black_ox_statue;      // TODO: IMPL
    } general;

    struct brewmaster_t
    {
      player_talent_t keg_smash;                      // TODO: IMPL // ROW 1
      player_talent_t purifying_brew;                 // TODO: IMPL // ROW 2
      player_talent_t shuffle;                        // TODO: IMPL
      player_talent_t staggering_strikes;             // TODO: IMPL // ROW 3
      player_talent_t gift_of_the_ox;                 // TODO: IMPL
      player_talent_t spirit_of_the_ox;               // TODO: IMPL
      player_talent_t quick_sip;                      // TODO: IMPL
      player_talent_t hit_scheme;                     // TODO: IMPL // ROW 4
      player_talent_t healing_elixir;                 // TODO: IMPL
      player_talent_t special_delivery;               // TODO: IMPL
      player_talent_t rushing_jade_wind;              // TODO: IMPL
      player_talent_t celestial_flames;               // TODO: IMPL // ROW 5
      player_talent_t celestial_brew;                 // TODO: IMPL
      player_talent_t graceful_exit;                  // TODO: IMPL
      player_talent_t zen_meditation;                 // TODO: IMPL
      player_talent_t clash;                          // TODO: IMPL
      player_talent_t breath_of_fire;                 // TODO: IMPL // ROW 6
      player_talent_t improved_celestial_brew;        // TODO: IMPL
      player_talent_t improved_purifing_brew;         // TODO: IMPL
      player_talent_t tranquil_spirit;                // TODO: IMPL
      player_talent_t gai_plins_imperial_brew;        // TODO: IMPL
      player_talent_t fundamental_observation;        // TODO: IMPL
      player_talent_t shadowboxing_treads;            // TODO: IMPL
      player_talent_t fluidity_of_motion;             // TODO: IMPL
      player_talent_t scalding_brew;                  // TODO: IMPL // ROW 7
      player_talent_t salsalabims_strength;           // TODO: IMPL
      player_talent_t fortifying_brew_determination;  // TODO: IMPL
      player_talent_t bob_and_weave;                  // TODO: IMPL
      player_talent_t black_ox_brew;                  // TODO: IMPL
      player_talent_t invoke_niuzao_the_black_ox;     // TODO: IMPL
      player_talent_t light_brewing;                  // TODO: IMPL
      player_talent_t training_of_niuzao;             // TODO: IMPL
      player_talent_t pretense_of_instability;        // TODO: IMPL
      player_talent_t counterstrike;                  // TODO: IMPL
      player_talent_t dragonfire_brew;                // TODO: IMPL // ROW 8
      player_talent_t charred_passions;               // TODO: IMPL
      player_talent_t high_tolerance;                 // TODO: IMPL
      player_talent_t walk_with_the_ox;               // TODO: IMPL
      player_talent_t elusive_footwork;               // TODO: IMPL
      player_talent_t anvil_and_stave;                // TODO: IMPL
      player_talent_t face_palm;                      // TODO: IMPL
      player_talent_t bonedust_brew;                  // TODO: IMPL // ROW 9
      player_talent_t improved_niuzao_the_black_ox;   // TODO: IMPL
      player_talent_t exploding_keg;                  // TODO: IMPL
      player_talent_t blackout_combo;                 // TODO: IMPL
      player_talent_t press_the_advantage;            // TODO: IMPL
      player_talent_t weapons_of_order;               // TODO: IMPL
      player_talent_t bountiful_brew;                 // TODO: IMPL // ROW 10
      player_talent_t attenuation;                    // TODO: IMPL
      player_talent_t stormstouts_last_keg;           // TODO: IMPL
      player_talent_t call_to_arms;                   // TODO: IMPL
      player_talent_t chi_surge;                      // TODO: IMPL
    } brewmaster;

    struct windwalker_t
    {
      player_talent_t fists_of_fury;                // TODO: IMPL // ROW 1
      player_talent_t touch_of_karma;               // TODO: IMPL // ROW 2
      player_talent_t ascension;                    // TODO: IMPL
      player_talent_t power_strikes;                // TODO: IMPL
      player_talent_t widening_whirl;               // TODO: IMPL // ROW 3
      player_talent_t touch_of_the_tiger;           // TODO: IMPL
      player_talent_t hardened_soles;               // TODO: IMPL
      player_talent_t flashing_fists;               // TODO: IMPL
      player_talent_t open_palm_strikes;            // TODO: IMPL
      player_talent_t mark_of_the_crane;            // TODO: IMPL // ROW 4
      player_talent_t flying_serpent_kick;          // TODO: IMPL
      player_talent_t glory_of_the_dawn;            // TODO: IMPL
      player_talent_t shadowboxing_treads;          // TODO: IMPL // ROW 5
      player_talent_t inner_peace;                  // TODO: IMPL
      player_talent_t storm_earth_and_fire;         // TODO: IMPL
      player_talent_t serenity;                     // TODO: IMPL
      player_talent_t meridian_strikes;             // TODO: IMPL
      player_talent_t strike_of_the_windlord;       // TODO: IMPL
      player_talent_t dance_of_chiji;               // TODO: IMPL // ROW 6
      player_talent_t jade_ignition;                // TODO: IMPL
      player_talent_t drinking_horn_cover;          // TODO: IMPL
      player_talent_t spiritual_focus;              // TODO: IMPL
      player_talent_t hit_combo;                    // TODO: IMPL
      player_talent_t rushing_jade_wind;            // TODO: IMPL // ROW 7
      player_talent_t forbidden_technique;          // TODO: IMPL
      player_talent_t invoke_xuen_the_white_tiger;  // TODO: IMPL
      player_talent_t teachings_of_the_monastery;   // TODO: IMPL
      player_talent_t thunderfist;                  // TODO: IMPL
      player_talent_t crane_vortex;                 // TODO: IMPL // ROW 8
      player_talent_t xuens_bond;                   // TODO: IMPL
      player_talent_t fury_of_xuen;                 // TODO: IMPL
      player_talent_t empowered_tiger_lightning;    // TODO: IMPL
      player_talent_t rising_star;                  // TODO: IMPL
      player_talent_t bonedust_brew;                // TODO: IMPL // ROW 9
      player_talent_t fatal_flying_guillotine;      // TODO: IMPL
      player_talent_t last_emperors_capacitor;      // TODO: IMPL
      player_talent_t xuens_battlegear;             // TODO: IMPL
      player_talent_t transfer_the_power;           // TODO: IMPL
      player_talent_t whirling_dragon_punch;        // TODO: IMPL
      player_talent_t faeline_stomp;                // TODO: IMPL
      player_talent_t attenuation;                  // TODO: IMPL // ROW 10
      player_talent_t dust_in_the_wind;             // TODO: IMPL
      player_talent_t skytouch;                     // TODO: IMPL
      player_talent_t skyreach;                     // TODO: IMPL
      player_talent_t invokers_delight;             // TODO: IMPL
      player_talent_t way_of_the_fae;               // TODO: IMPL
      player_talent_t faeline_harmony;              // TODO: IMPL
    } windwalker;

    struct mistweaver_t
    {
      player_talent_t enveloping_mist;                // TODO: IMPL // ROW 1
      player_talent_t essence_font;                   // TODO: IMPL // ROW 2
      player_talent_t renewing_mist;                  // TODO: IMPL
      player_talent_t life_cocoon;                    // TODO: IMPL // ROW 3
      player_talent_t thunder_focus_tea;              // TODO: IMPL
      player_talent_t mana_tea;                       // TODO: IMPL
      player_talent_t teachings_of_the_monastery;     // TODO: IMPL // ROW 4
      player_talent_t revival;                        // TODO: IMPL
      player_talent_t restoral;                       // TODO: IMPL
      player_talent_t invigorating_mists;             // TODO: IMPL
      player_talent_t nourishing_chi;                 // TODO: IMPL // ROW 5
      player_talent_t calming_coalescence;            // TODO: IMPL
      player_talent_t uplifted_spirits;               // TODO: IMPL
      player_talent_t font_of_life;                   // TODO: IMPL
      player_talent_t zen_pulse;                      // TODO: IMPL
      player_talent_t healing_elixir;                 // TODO: IMPL
      player_talent_t mists_of_life;                  // TODO: IMPL // ROW 6
      player_talent_t overflowing_mists;              // TODO: IMPL
      player_talent_t invoke_yulon_the_jade_serpent;  // TODO: IMPL
      player_talent_t invoke_chiji_the_red_crane;     // TODO: IMPL
      player_talent_t echoing_reverberation;          // TODO: IMPL
      player_talent_t accumulating_mist;              // TODO: IMPL
      player_talent_t rapid_diffusion;                // TODO: IMPL
      player_talent_t chrysalis;                      // TODO: IMPL // ROW 7
      player_talent_t burst_of_life;                  // TODO: IMPL
      player_talent_t yulons_whisper;                 // TODO: IMPL
      player_talent_t mist_wrap;                      // TODO: IMPL
      player_talent_t refreshing_jade_wind;           // TODO: IMPL
      player_talent_t celestial_harmony;              // TODO: IMPL
      player_talent_t dancing_mists;                  // TODO: IMPL
      player_talent_t energizing_brew;                // TODO: IMPL
      player_talent_t lifecycles;                     // TODO: IMPL
      player_talent_t faeline_stomp;                  // TODO: IMPL // ROW 8
      player_talent_t ancient_teachings;              // TODO: IMPL
      player_talent_t clouded_focus;                  // TODO: IMPL
      player_talent_t jade_bond;                      // TODO: IMPL
      player_talent_t gift_of_the_celestials;         // TODO: IMPL
      player_talent_t focused_thunder;                // TODO: IMPL
      player_talent_t upwelling;                      // TODO: IMPL
      player_talent_t sheiluns_gift;                  // TODO: IMPL
      player_talent_t ancient_concordance;            // TODO: IMPL // ROW 9
      player_talent_t resplendent_mist;               // TODO: IMPL
      player_talent_t secret_infusion;                // TODO: IMPL
      player_talent_t misty_peaks;                    // TODO: IMPL
      player_talent_t peaceful_mending;               // TODO: IMPL
      player_talent_t veil_of_pride;                  // TODO: IMPL
      player_talent_t shaohaos_lessons;               // TODO: IMPL
      player_talent_t awakned_faeline;                // TODO: IMPL // ROW 10
      player_talent_t tea_of_serenity;                // TODO: IMPL
      player_talent_t tea_of_plenty;                  // TODO: IMPL
      player_talent_t unison;                         // TODO: IMPL
      player_talent_t mending_proliferation;          // TODO: IMPL
      player_talent_t invokers_delight;               // TODO: IMPL
      player_talent_t tear_of_morning;                // TODO: IMPL
      player_talent_t rising_mist;                    // TODO: IMPL
      player_talent_t legacy_of_wisdom;               // TODO: IMPL
    } mistweaver;
  } talent;

  // Masteries
  struct mastery_t
  {
    const spell_data_t* elusive_brawler;  // TODO: IMPL
    const spell_data_t* combo_strikes;    // TODO: IMPL
    const spell_data_t* gust_of_mists;    // TODO: IMPL
  } mastery;

  // Would a better name for this be "triggered_actions_t"? "active_action_t" is
  // somewhat ambiguous.
  // Actions that are needed to be triggered by other actions.
  struct trigger_action_t
  {
    propagate_const<chi_surge_t*> chi_surge;
  } action;

  // Player Buffs
  struct buff_t
  {
  } buff;

  // Player Debuffs
  struct debuff_t
  {
  } debuff;

  // Procs
  struct proc_t
  {
  } proc;

  // RPPM Procs
  struct rppm_t
  {
  } rppm;

  // Resource Gains
  struct gain_t
  {
  } gain;

  // Cooldowns
  struct cooldowns_t
  {
    propagate_const<cooldown_t*> weapons_of_order;
  } cooldown;

  // Pets
  // Note: does pet need `this` passed into it?
  struct pet_t
  {
    pet_t( monk_t* /* p */ )
    {
    }
  } pet;

  // Sim Options
  struct options_t
  {
  } sim_options;

  monk_t( sim_t* sim, std::string_view name, race_e race );

  void apply_affecting_auras( action_t& action ) override;
  stat_e convert_hybrid_stat( stat_e stat ) const override;
  void copy_from( player_t* source ) override;
  action_t* create_action( std::string_view name, std::string_view options_str ) override;
  void create_options() override;
  std::string default_flask() const override;
  std::string default_food() const override;
  std::string default_potion() const override;
  std::string default_rune() const override;
  std::string default_temporary_enchant() const override;
  void init_base_stats() override;
  void init_rng() override;
  void init_scaling() override;
  void init_spells() override;
  void invalidate_cache( cache_e cache ) override;
  double matching_gear_multiplier( attribute_e attribute ) const override;
  void merge( player_t& other ) override;
  resource_e primary_resource() const override;
  role_e primary_role() const override;
  void reset() override;
};

struct monk_td_t : public actor_target_data_t
{
  struct dot_t
  {
  } dot;

  struct debuff_t
  {
  } debuff;

  monk_t* player;

  monk_td_t( player_t* target, monk_t* player );
};

struct monk_report_t : public player_report_extension_t
{
  struct bug_t
  {
    std::string_view description;
    std::string_view date;
    bool match;

    bug_t( std::string_view description, std::string_view date, bool match );
  };

  std::vector<bug_t*> issues;
  monk_t& player;

  monk_report_t( monk_t& player );

  void bug_report( report::sc_html_stream& os );
  void html_customsection( report::sc_html_stream& os ) override;
};

struct monk_module_t : public module_t
{
  monk_module_t();

  player_t* create_player( sim_t* sim, std::string_view name, race_e race ) const override;
  bool valid() const override;
  void static_init() const override;
  void register_hotfixes() const override;
  void init( player_t* ) const override;
  void combat_begin( sim_t* ) const override;
  void combat_end( sim_t* ) const override;
};

}  // namespace monk
