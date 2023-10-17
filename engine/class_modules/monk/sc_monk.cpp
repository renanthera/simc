// ==========================================================================
// Dedmonwakeen's DPS-DPM Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include "action/action_callback.hpp"
#include "action/parse_buff_effects.hpp"
#include "class_modules/apl/apl_monk.hpp"
#include "player/pet.hpp"
#include "player/pet_spawner.hpp"
#include "report/charts.hpp"
#include "report/highchart.hpp"
#include "sc_enums.hpp"

#include "simulationcraft.hpp"

namespace
{
struct monk_t : public player_t
{
public:
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
  struct active_action_t
  {
  } active_action;

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

  // Internal Cooldowns
  struct cooldown_t
  {
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

  monk_t( sim_t* sim, std::string_view name, race_e race )
    : player_t( sim, MONK, name, race ),
      sample_data( sample_data_t() ),
      ability(),
      passive(),
      talent(),
      mastery(),
      active_action(),
      buff(),
      debuff(),
      proc(),
      rppm(),
      gain(),
      cooldown(),
      pet( this ),
      sim_options()
  {
  }

  action_t* create_action( std::string_view name, std::string_view options_str ) override;

  void merge( player_t& other ) override
  {
    player_t::merge( other );
    // const monk_t& other_monk = static_cast<const monk_t&>( other );
  }

  role_e primary_role() const override
  {
    switch ( player_t::primary_role() )
    {
      case ROLE_NONE:
      case ROLE_ATTACK:
      case ROLE_SPELL:
      case ROLE_HYBRID:
      case ROLE_DPS:
      case ROLE_TANK:
      case ROLE_HEAL:
        return player_t::primary_role();
      case ROLE_MAX:
      default:
        break;
    }

    switch ( specialization() )
    {
      case MONK_BREWMASTER:
        return ROLE_TANK;
      case MONK_MISTWEAVER:
      case MONK_WINDWALKER:
      default:
        return ROLE_ATTACK;
    }
  }

  void init_spells() override
  {
    player_t::init_spells();

    auto class_talent = [ this ]( std::string_view name ) { return find_talent_spell( talent_tree::CLASS, name ); };
    auto spec_talent  = [ this ]( std::string_view name ) {
      return find_talent_spell( talent_tree::SPECIALIZATION, name );
    };

    // TODO: sample_data_t initialization
    // ability_t::general_t initialization
    {
      if ( specialization() == MONK_BREWMASTER )
        ability.general.blackout_kick = find_spell( 205523 );
      else
        ability.general.blackout_kick = find_class_spell( "Blackout Kick" );
    }
    // ability_t::brewmaster_t initialization
    {}

    // passive_t::general_t initialization
    {
      passive.general.class_aura = find_spell( 137022 );
    }
    // passive_t::brewmaster_t initialization
    {
      passive.brewmaster.spec_aura              = find_specialization_spell( "Brewmaster Monk" );
      passive.brewmaster.leather_specialization = find_specialization_spell( "Leather Specialization" );
    }
    // passive_t::windwalker_t initialization
    {
      passive.windwalker.leather_specialization = find_specialization_spell( "Leather Specialization" );
    }
    // passive_t::mistweaver_t initialization
    {
      passive.mistweaver.leather_specialization = find_specialization_spell( "Leather Specialization" );
    }

    // talent_t::general_t initialization
    {
      talent.general.soothing_mist              = class_talent( "Soothing Mist" );
      talent.general.rising_sun_kick            = class_talent( "Rising Sun Kick" );
      talent.general.tigers_lust                = class_talent( "Tiger's Lust" );
      talent.general.improved_roll              = class_talent( "Improved Roll" );
      talent.general.calming_presence           = class_talent( "Calming Presence" );
      talent.general.disable                    = class_talent( "Disable" );
      talent.general.tiger_tail_sweep           = class_talent( "Tiger Tail Sweep" );
      talent.general.vigorous_expulsion         = class_talent( "Vigorous Expulsion" );
      talent.general.improved_vivify            = class_talent( "Improved Vivify" );
      talent.general.detox                      = class_talent( "Detox" );
      talent.general.improved_detox             = class_talent( "Improved Detox" );
      talent.general.paralysis                  = class_talent( "Paralysis" );
      talent.general.grace_of_the_crane         = class_talent( "Grace of the Crane" );
      talent.general.vivacious_vivification     = class_talent( "Vivacious Vivification" );
      talent.general.ferocity_of_xuen           = class_talent( "Ferocity of Xuen" );
      talent.general.improved_paralysis         = class_talent( "Improved Paralysis" );
      talent.general.elusive_mists              = class_talent( "Elusive Mists" );
      talent.general.transcendence              = class_talent( "Transcendence" );
      talent.general.spear_hand_strike          = class_talent( "Spear Hand Strike" );
      talent.general.fortifying_brew            = class_talent( "Fortifying Brew" );
      talent.general.chi_wave                   = class_talent( "Chi Wave" );
      talent.general.chi_burst                  = class_talent( "Chi Burst" );
      talent.general.hasty_provocation          = class_talent( "Hasty Provocation" );
      talent.general.ring_of_peace              = class_talent( "Ring of Peace" );
      talent.general.song_of_chiji              = class_talent( "Song of Chi-Ji" );
      talent.general.fast_feet                  = class_talent( "Fast Feet" );
      talent.general.celerity                   = class_talent( "Celerity" );
      talent.general.chi_torpedo                = class_talent( "Chi Torpedo" );
      talent.general.ironshell_brew             = class_talent( "Ironshell Brew" );
      talent.general.expeditious_fortification  = class_talent( "Expeditious Fortification" );
      talent.general.profound_rebuttal          = class_talent( "Profound Rebuttal" );
      talent.general.yulons_grace               = class_talent( "Yu'lon's Grace" );
      talent.general.diffuse_magic              = class_talent( "Diffuse Magic" );
      talent.general.eye_of_the_tiger           = class_talent( "Eye of the Tiger" );
      talent.general.dance_of_the_wind          = class_talent( "Dance of the Wind" );
      talent.general.dampen_harm                = class_talent( "Dampen Harm" );
      talent.general.improved_touch_of_death    = class_talent( "Improved Touch of Death" );
      talent.general.strength_of_spirit         = class_talent( "Strength of Spirit" );
      talent.general.close_to_heart             = class_talent( "Close to Heart" );
      talent.general.escape_from_reality        = class_talent( "Escape from Reality" );
      talent.general.windwalking                = class_talent( "Windwalking" );
      talent.general.fatal_touch                = class_talent( "Fatal Touch" );
      talent.general.generous_pour              = class_talent( "Generous Pour" );
      talent.general.save_them_all              = class_talent( "Save Them All" );
      talent.general.resonant_fists             = class_talent( "Resonant Fists" );
      talent.general.bounce_back                = class_talent( "Bounce Back" );
      talent.general.summon_jade_serpent_statue = class_talent( "Summon Jade Serpent Statue" );
      talent.general.summon_white_tiger_statue  = class_talent( "Summon White Tiger Statue" );
      talent.general.summon_black_ox_statue     = class_talent( "Summon Black Ox Statue" );
    }

    // talent_t::brewmaster_t initialization
    // TODO: Investigate whether wrapping spec talent initialization blocks
    // yields performance improvements.
    {
      talent.brewmaster.keg_smash                     = spec_talent( "Keg Smash" );
      talent.brewmaster.purifying_brew                = spec_talent( "Purifying Brew" );
      talent.brewmaster.shuffle                       = spec_talent( "Shuffle" );
      talent.brewmaster.staggering_strikes            = spec_talent( "Staggering Strikes" );
      talent.brewmaster.gift_of_the_ox                = spec_talent( "Gift of the Ox" );
      talent.brewmaster.spirit_of_the_ox              = spec_talent( "Spirit of the Ox" );
      talent.brewmaster.quick_sip                     = spec_talent( "Quick Sip" );
      talent.brewmaster.hit_scheme                    = spec_talent( "Hit Scheme" );
      talent.brewmaster.healing_elixir                = spec_talent( "Healing Elixir" );
      talent.brewmaster.special_delivery              = spec_talent( "Special Delivery" );
      talent.brewmaster.rushing_jade_wind             = spec_talent( "Rushing Jade Wind" );
      talent.brewmaster.celestial_flames              = spec_talent( "Celestial Flames" );
      talent.brewmaster.celestial_brew                = spec_talent( "Celestial Brew" );
      talent.brewmaster.graceful_exit                 = spec_talent( "Graceful Exit" );
      talent.brewmaster.zen_meditation                = spec_talent( "Zen Meditation" );
      talent.brewmaster.clash                         = spec_talent( "Clash" );
      talent.brewmaster.breath_of_fire                = spec_talent( "Breath of Fire" );
      talent.brewmaster.improved_celestial_brew       = spec_talent( "Improved Celestial Brew" );
      talent.brewmaster.improved_purifing_brew        = spec_talent( "Improved Purifying Brew" );
      talent.brewmaster.tranquil_spirit               = spec_talent( "Tranquil Spirit" );
      talent.brewmaster.gai_plins_imperial_brew       = spec_talent( "Gai Plin's Imperial Brew" );
      talent.brewmaster.fundamental_observation       = spec_talent( "Fundamental Observation" );
      talent.brewmaster.shadowboxing_treads           = spec_talent( "Shadowboxing Treads" );
      talent.brewmaster.fluidity_of_motion            = spec_talent( "Fluidity of Motion" );
      talent.brewmaster.scalding_brew                 = spec_talent( "Scalding Brew" );
      talent.brewmaster.salsalabims_strength          = spec_talent( "Sal'salabim's Strength" );
      talent.brewmaster.fortifying_brew_determination = spec_talent( "Fortifying Brew: Determination" );
      talent.brewmaster.bob_and_weave                 = spec_talent( "Bob and Weave" );
      talent.brewmaster.black_ox_brew                 = spec_talent( "Black Ox Brew" );
      talent.brewmaster.invoke_niuzao_the_black_ox    = spec_talent( "Invoke Niuzao, the Black Ox" );
      talent.brewmaster.light_brewing                 = spec_talent( "Light Brewing" );
      talent.brewmaster.training_of_niuzao            = spec_talent( "Training of Niuzao" );
      talent.brewmaster.pretense_of_instability       = spec_talent( "Pretense of Instability" );
      talent.brewmaster.counterstrike                 = spec_talent( "Counterstrike" );
      talent.brewmaster.dragonfire_brew               = spec_talent( "Dragonfire Brew" );
      talent.brewmaster.charred_passions              = spec_talent( "Charred Passions" );
      talent.brewmaster.high_tolerance                = spec_talent( "High Tolerance" );
      talent.brewmaster.walk_with_the_ox              = spec_talent( "Walk With the Ox" );
      talent.brewmaster.elusive_footwork              = spec_talent( "Elusive Footwork" );
      talent.brewmaster.anvil_and_stave               = spec_talent( "Anvil & Stave" );
      talent.brewmaster.face_palm                     = spec_talent( "Face Palm" );
      talent.brewmaster.bonedust_brew                 = spec_talent( "Bonedust Brew" );
      talent.brewmaster.improved_niuzao_the_black_ox  = spec_talent( "Improved Niuzao, the Black Ox" );
      talent.brewmaster.exploding_keg                 = spec_talent( "Exploding Keg" );
      talent.brewmaster.blackout_combo                = spec_talent( "Blackout Combo" );
      talent.brewmaster.press_the_advantage           = spec_talent( "Press the Advantage" );
      talent.brewmaster.weapons_of_order              = spec_talent( "Weapons of Order" );
      talent.brewmaster.bountiful_brew                = spec_talent( "Bountiful Brew" );
      talent.brewmaster.attenuation                   = spec_talent( "Attenuation" );
      talent.brewmaster.stormstouts_last_keg          = spec_talent( "Stormstout's Last Keg" );
      talent.brewmaster.call_to_arms                  = spec_talent( "Call to Arms" );
      talent.brewmaster.chi_surge                     = spec_talent( "Chi Surge" );
    }

    // talent_t::windwalker_t initialization
    {
      talent.windwalker.fists_of_fury               = spec_talent( "Fists of Fury" );
      talent.windwalker.touch_of_karma              = spec_talent( "Touch of Karma" );
      talent.windwalker.ascension                   = spec_talent( "Ascension" );
      talent.windwalker.power_strikes               = spec_talent( "Power Strikes" );
      talent.windwalker.widening_whirl              = spec_talent( "Widening Whirl" );
      talent.windwalker.touch_of_the_tiger          = spec_talent( "Touch of the Tiger" );
      talent.windwalker.hardened_soles              = spec_talent( "Hardened Soles" );
      talent.windwalker.flashing_fists              = spec_talent( "Flashing Fists" );
      talent.windwalker.open_palm_strikes           = spec_talent( "Open Palm Strikes" );
      talent.windwalker.mark_of_the_crane           = spec_talent( "Mark of the Crane" );
      talent.windwalker.flying_serpent_kick         = spec_talent( "Flying Serpent Kick" );
      talent.windwalker.glory_of_the_dawn           = spec_talent( "Glory of the Dawn" );
      talent.windwalker.shadowboxing_treads         = spec_talent( "Shadowboxing Treads" );
      talent.windwalker.inner_peace                 = spec_talent( "Inner Peace" );
      talent.windwalker.storm_earth_and_fire        = spec_talent( "Storm Earth and Fire" );
      talent.windwalker.serenity                    = spec_talent( "Serenity" );
      talent.windwalker.meridian_strikes            = spec_talent( "Meridian Strikes" );
      talent.windwalker.strike_of_the_windlord      = spec_talent( "Strike of the Windlord" );
      talent.windwalker.dance_of_chiji              = spec_talent( "Dance of Chi-Ji" );
      talent.windwalker.jade_ignition               = spec_talent( "Jade Ignition" );
      talent.windwalker.drinking_horn_cover         = spec_talent( "Drinking Horn Cover" );
      talent.windwalker.spiritual_focus             = spec_talent( "Spiritual Focus" );
      talent.windwalker.hit_combo                   = spec_talent( "Hit Combo" );
      talent.windwalker.rushing_jade_wind           = spec_talent( "Rushing Jade Wind" );
      talent.windwalker.forbidden_technique         = spec_talent( "Forbidden Technique" );
      talent.windwalker.invoke_xuen_the_white_tiger = spec_talent( "Invoke Xuen, the White Tiger" );
      talent.windwalker.teachings_of_the_monastery  = spec_talent( "Teachings of the Monastery" );
      talent.windwalker.thunderfist                 = spec_talent( "Thunderfist" );
      talent.windwalker.crane_vortex                = spec_talent( "Crane Vortex" );
      talent.windwalker.xuens_bond                  = spec_talent( "Xuen's Bond" );
      talent.windwalker.fury_of_xuen                = spec_talent( "Fury of Xuen" );
      talent.windwalker.empowered_tiger_lightning   = spec_talent( "Empowered Tiger Lightning" );
      talent.windwalker.rising_star                 = spec_talent( "Rishing Star" );
      talent.windwalker.bonedust_brew               = spec_talent( "Bonedust Brew" );
      talent.windwalker.fatal_flying_guillotine     = spec_talent( "Fatal Flying Guillotine" );
      talent.windwalker.last_emperors_capacitor     = spec_talent( "Last Emperor's Capacitor" );
      talent.windwalker.xuens_battlegear            = spec_talent( "Xuen's Battlegear" );
      talent.windwalker.transfer_the_power          = spec_talent( "Transfer the Power" );
      talent.windwalker.whirling_dragon_punch       = spec_talent( "Whirling Dragon Punch" );
      talent.windwalker.faeline_stomp               = spec_talent( "Faeline Stomp" );
      talent.windwalker.attenuation                 = spec_talent( "Attenuation" );
      talent.windwalker.dust_in_the_wind            = spec_talent( "Dust in the Wind" );
      talent.windwalker.skytouch                    = spec_talent( "Skytouch" );
      talent.windwalker.skyreach                    = spec_talent( "Skyreach" );
      talent.windwalker.invokers_delight            = spec_talent( "Invoker's Delight" );
      talent.windwalker.way_of_the_fae              = spec_talent( "Way of the Fae" );
      talent.windwalker.faeline_harmony             = spec_talent( "Faeline Harmony" );
    }

    // TODO: talent_t::mistweaver_t initialization
    // TODO: mastery_t initialization
    // TODO: active_action_t initialization
    // TODO: buff_t initialization
    // TODO: debuff_t initialization
    // TODO: proc_t initialization
    // TODO: rppm_t initialization
    // TODO: gain_t initialization
    // TODO: cooldown_t initialization
    // TODO: pet_t initialization
  }

  void apply_affecting_auras( action_t& action ) override
  {
    player_t::apply_affecting_auras( action );

    // Class
    action.apply_affecting_aura( passive.general.class_aura );

    // Brewmaster
    action.apply_affecting_aura( passive.brewmaster.spec_aura );
  }

  void init_base_stats() override
  {
    // where is WW gcd adjustment?
    if ( base.distance < 1 )
      base.distance = specialization() == MONK_MISTWEAVER ? 40 : 5;

    player_t::init_base_stats();

    base_gcd = 1.5_s;
  }

  void init_scaling() override
  {
    // TODO: VERIFY SC
    // Why did only WW get Stamina? All three specs have ToD.
    player_t::init_scaling();

    scaling->disable( STAT_STRENGTH );
    scaling->enable( STAT_STAMINA );

    if ( off_hand_weapon.type != WEAPON_NONE )
      scaling->enable( STAT_WEAPON_OFFHAND_DPS );

    switch ( specialization() )
    {
      case MONK_BREWMASTER:
        scaling->enable( STAT_BONUS_ARMOR );
        [[fallthrough]];
      case MONK_WINDWALKER:
        scaling->disable( STAT_INTELLECT );
        scaling->disable( STAT_SPELL_POWER );
        scaling->enable( STAT_AGILITY );
        scaling->enable( STAT_WEAPON_DPS );
        break;
      case MONK_MISTWEAVER:
      default:
        scaling->disable( STAT_AGILITY );
        scaling->disable( STAT_MASTERY_RATING );
        scaling->disable( STAT_ATTACK_POWER );
        scaling->disable( STAT_WEAPON_DPS );
        scaling->enable( STAT_INTELLECT );
        scaling->enable( STAT_SPIRIT );
        break;
    }
  }

  void init_rng() override
  {
    // TODO: STUB
    player_t::init_rng();
  }

  void reset() override
  {
    // TODO: STUB
    player_t::reset();

    if ( sim->debug )
    {
      // std::ostream* stream = sim->out_debug.raw().get_stream();
    }
  }

  double matching_gear_multiplier( attribute_e attribute ) const override
  {
    switch ( attribute )
    {
      case ATTR_AGILITY:
        return passive.windwalker.leather_specialization->effectN( 1 ).percent();
      case ATTR_INTELLECT:
        return passive.mistweaver.leather_specialization->effectN( 1 ).percent();
      case ATTR_STAMINA:
        return passive.brewmaster.leather_specialization->effectN( 1 ).percent();
      default:
        return 0.0;
    }
  }

  void invalidate_cache( cache_e cache ) override
  {
    player_t::invalidate_cache( cache );

    switch ( cache )
    {
      case CACHE_ATTACK_POWER:
      case CACHE_AGILITY:
        switch ( specialization() )
        {
          case MONK_BREWMASTER:
          case MONK_WINDWALKER:
            player_t::invalidate_cache( CACHE_SPELL_POWER );
            break;
          default:
            break;
        }
        break;
      case CACHE_SPELL_POWER:
      case CACHE_INTELLECT:
        if ( specialization() == MONK_MISTWEAVER )
          player_t::invalidate_cache( CACHE_ATTACK_POWER );
        break;
      default:
        break;
    }
  }

  void copy_from( player_t* source ) override
  {
    // TODO: INACTIVE
    player_t::copy_from( source );
    // monk_t* source_p = debug_cast<monk_t*>( source );

    // user_options = source_p->user_options;
  }

  void create_options() override
  {
    player_t::create_options();

    // add_option( opt_int( "monk.initial_chi", user_options.initial_chi, 0, 6 ) );
    // add_option( opt_float( "monk.expel_harm_effectiveness", user_options.expel_harm_effectiveness, 0.0, 1.0 ) );
    // add_option( opt_float( "monk.faeline_stomp_uptime", user_options.faeline_stomp_uptime, 0.0, 1.0 ) );
    // add_option( opt_int( "monk.chi_burst_healing_targets", user_options.chi_burst_healing_targets, 0, 30 ) );
    // add_option( opt_int( "monk.motc_override", user_options.motc_override, 0, 5 ) );
    // add_option( opt_float( "monk.squirm_frequency", user_options.squirm_frequency, 0, 30 ) );
  }

  resource_e primary_resource() const override
  {
    switch ( specialization() )
    {
      case MONK_BREWMASTER:
      case MONK_WINDWALKER:
        return RESOURCE_ENERGY;
      case MONK_MISTWEAVER:
        return RESOURCE_MANA;
      default:
        return RESOURCE_NONE;
    }
  }

  stat_e convert_hybrid_stat( stat_e stat ) const override
  {
    // TODO: VERIFY IG
    // TODO: VERIFY SC
    switch ( stat )
    {
      case STAT_STR_AGI_INT:
      case STAT_AGI_INT:
        switch ( specialization() )
        {
          case MONK_BREWMASTER:
          case MONK_WINDWALKER:
            return STAT_AGILITY;
          case MONK_MISTWEAVER:
            return STAT_INTELLECT;
          default:
            return STAT_NONE;
        }
      case STAT_STR_AGI:
        switch ( specialization() )
        {
          case MONK_BREWMASTER:
          case MONK_WINDWALKER:
            return STAT_AGILITY;
          case MONK_MISTWEAVER:
            return STAT_NONE;  // does this need to be agi? following impl implies no
          default:
            return STAT_NONE;
        }
      case STAT_STR_INT:
        switch ( specialization() )
        {
          case MONK_BREWMASTER:
          case MONK_WINDWALKER:
            return STAT_NONE;
          case MONK_MISTWEAVER:
            return STAT_INTELLECT;
          default:
            return STAT_NONE;
        }
      case STAT_SPIRIT:
        switch ( specialization() )
        {
          case MONK_BREWMASTER:
          case MONK_WINDWALKER:
            return STAT_NONE;
          case MONK_MISTWEAVER:
            return STAT_SPIRIT;
          default:
            return STAT_NONE;
        }
      case STAT_BONUS_ARMOR:
        switch ( specialization() )
        {
          case MONK_BREWMASTER:
            return STAT_BONUS_ARMOR;
          case MONK_WINDWALKER:
          case MONK_MISTWEAVER:
          default:
            return STAT_NONE;
        }
      default:
        return stat;
    }
  }

  std::string default_flask() const override
  {
    return monk_apl::flask( this );
  }

  std::string default_potion() const override
  {
    return monk_apl::potion( this );
  }

  std::string default_food() const override
  {
    return monk_apl::food( this );
  }

  std::string default_rune() const override
  {
    return monk_apl::rune( this );
  }

  std::string default_temporary_enchant() const override
  {
    return monk_apl::temporary_enchant( this );
  }
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

  monk_td_t( player_t* target, monk_t* player )
    : actor_target_data_t( target, player ), dot(), debuff(), player( player )
  {
  }
};

template <class Base>
struct monk_action_t : public Base, public parse_buff_effects_t<monk_td_t>
{
public:
  using base_t = monk_action_t<Base>;
  monk_action_t( std::string_view name, monk_t* player, const spell_data_t* spell_data = spell_data_t::nil() )
    : Base( name, player, spell_data ), parse_buff_effects_t( this )
  {
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
};

struct monk_attack_t : public monk_action_t<melee_attack_t>
{
  monk_attack_t( std::string_view name, monk_t* player, const spell_data_t* spell = spell_data_t::nil() )
    : base_t( name, player, spell )
  {
    special        = true;
    may_glance     = false;
    track_cd_waste = true;
  }
};

// Ability: Auto Attack
struct auto_attack_t : public monk_attack_t
{
  struct melee_t : public monk_attack_t
  {
    bool first;
    weapon_t* weapon;

    melee_t( std::string_view name, monk_t* player, weapon_t* weapon )
      : monk_attack_t( name, player ), first( true ), weapon( weapon )
    {
      background                = true;
      repeating                 = true;
      may_glance                = true;
      may_crit = true;
      special                   = false;
      trigger_gcd               = 0_s;
      school                    = SCHOOL_PHYSICAL;
      weapon_multiplier         = 1.0;
      allow_class_ability_procs = true;
      not_a_proc                = true;
      base_execute_time         = weapon->swing_time;

      if ( player->main_hand_weapon.group() == WEAPON_1H && player->specialization() != MONK_MISTWEAVER )
        base_hit -= 0.19;
    }
  };

  melee_t* main_hand;
  melee_t* off_hand;

  auto_attack_t( monk_t* player, std::string_view options_str ) : monk_attack_t( "melee", player )
  {
    parse_options( options_str );
    background            = true;
    special               = false;
    ignore_false_positive = true;  // disables "player skill" configuration
    trigger_gcd           = 0_s;

    main_hand = new melee_t( "melee_main_hand", player, &( player->main_hand_weapon ) );
    add_child( main_hand );
    if ( player->off_hand_weapon.type != WEAPON_NONE && player->dual_wield() )
    {
      off_hand = new melee_t( "melee_off_hand", player, &( player->off_hand_weapon ) );
      add_child( off_hand );
    }
  }

  bool ready() override
  {
    if ( player->current.distance_to_move > 5 )
      return false;
    return ( main_hand->execute_event == nullptr );
  }

  void execute() override
  {
    if ( main_hand->execute_event == nullptr )
      main_hand->schedule_execute();
    if ( off_hand->execute_event == nullptr )
      off_hand->schedule_execute();
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

action_t* monk_t::create_action( std::string_view name, std::string_view options_str )
{
  // Baseline Abilities
  if ( name == "blackout_kick" )
    return new blackout_kick_t( this, options_str );
  if ( name == "auto_attack" )
    return new auto_attack_t( this, options_str );

  return player_t::create_action( name, options_str );
}

struct monk_report_t : public player_report_extension_t
{
  struct monk_bug
  {
    std::string_view description;
    std::string_view date;
    bool match;

    monk_bug( std::string description, std::string date, bool match )
      : description( description ), date( date ), match( match )
    {
    }
  };

  std::vector<monk_bug*> issues;

  monk_report_t( monk_t& player ) : p( player )
  {
  }

  void bug_report( report::sc_html_stream& os )
  {
    auto ReportIssue = [ this ]( std::string description, std::string date, bool match = false ) {
      issues.push_back( new monk_bug( description, date, match ) );
    };

    ReportIssue( "The spells that contribute to ETL change based on which buff(s) are up", "2023-08-01", true );
    ReportIssue( "The ETL cache for both tigers resets to 0 when either spawn", "2023-08-03", true );
    ReportIssue( "The spells that FoX contributes to ETL change after the first tick of damage", "2023-08-01", true );
    ReportIssue( "Xuen's Bond is triggering from SEF combo strikes", "2023-02-21", true );
    ReportIssue( "Jade Ignition is reduced by SEF but not copied", "2023-02-22", true );
    ReportIssue( "Blackout Combo buffs both the initial and periodic effect of Breath of Fire", "2023-03-08", true );

    os << "<div class=\"section\">\n";
    os << "<h2 class=\"toggle\">Known Bugs and Issues</h2>\n";
    os << "<div class=\"toggle-content hide\">\n";

    for ( monk_bug* issue : issues )
    {
      if ( issue->description.empty() )
        continue;

      os << "<h3>" << issue->description << "</h3>\n";

      os << "<table class=\"sc even\">\n"
         << "<thead>\n"
         << "<tr>\n"
         << "<th class=\"left\">Effective Date</th>\n"
         << "<th class=\"left\">Sim Matches Game Behavior</th>\n"
         << "</tr>\n"
         << "</thead>\n";

      os << "<tr>\n"
         << "<td class=\"left\"><strong>" << issue->date << "</strong></td>\n"
         << "<td class=\"left\" colspan=\"5\"><strong>" << ( issue->match ? "YES" : "NO" ) << "</strong></td>\n"
         << "</tr>\n";

      os << "</table>\n";
    }

    os << "</table>\n";
    os << "</div>\n";
    os << "</div>\n";
  }

  void html_customsection( report::sc_html_stream& os ) override
  {
    bug_report( os );

    (void)p;
  }

private:
  monk_t& p;
};

struct monk_module_t : public module_t
{
  monk_module_t() : module_t( MONK )
  {
  }

  player_t* create_player( sim_t* sim, std::string_view name, race_e race ) const override
  {
    monk_t* p           = new monk_t( sim, name, race );
    p->report_extension = std::make_unique<monk_report_t>( *p );

    return p;
  }

  bool valid() const override
  {
    return true;
  }

  void static_init() const override
  {
  }

  void register_hotfixes() const override
  {
  }

  void init( player_t* ) const override
  {
  }

  void combat_begin( sim_t* ) const override
  {
  }

  void combat_end( sim_t* ) const override
  {
  }
};
}  // namespace

const module_t* module_t::monk()
{
  static monk_module_t m;

  return &m;
}
