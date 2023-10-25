// ==========================================================================
// Dedmonwakeen's DPS-DPM Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include "sc_monk.hpp"

#include "action/action_callback.hpp"
#include "action/parse_buff_effects.hpp"
#include "class_modules/apl/apl_monk.hpp"
#include "player/pet.hpp"
#include "player/pet_spawner.hpp"
#include "report/charts.hpp"
#include "report/highchart.hpp"
#include "sc_enums.hpp"
#include "sc_monk_abilities.cpp"

#include "simulationcraft.hpp"

namespace monk
{
monk_t::monk_t( sim_t* sim, std::string_view name, race_e race )
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

void monk_t::merge( player_t& other )
{
  player_t::merge( other );
  // const monk_t& other_monk = static_cast<const monk_t&>( other );
}

role_e monk_t::primary_role() const
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

void monk_t::init_spells()
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
  {
    active_action.test = new blackout_kick_t( this, "" );
  }
  // TODO: buff_t initialization
  // TODO: debuff_t initialization
  // TODO: proc_t initialization
  // TODO: rppm_t initialization
  // TODO: gain_t initialization
  // TODO: cooldown_t initialization
  // TODO: pet_t initialization
}

void monk_t::apply_affecting_auras( action_t& action )
{
  player_t::apply_affecting_auras( action );

  // Class
  action.apply_affecting_aura( passive.general.class_aura );

  // Brewmaster
  action.apply_affecting_aura( passive.brewmaster.spec_aura );
}

void monk_t::init_base_stats()
{
  // where is WW gcd adjustment?
  if ( base.distance < 1 )
    base.distance = specialization() == MONK_MISTWEAVER ? 40 : 5;

  player_t::init_base_stats();

  base_gcd = 1.5_s;
}

void monk_t::init_scaling()
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

void monk_t::init_rng()
{
  // TODO: STUB
  player_t::init_rng();
}

void monk_t::reset()
{
  // TODO: STUB
  player_t::reset();

  if ( sim->debug )
  {
    // std::ostream* stream = sim->out_debug.raw().get_stream();
  }
}

double monk_t::matching_gear_multiplier( attribute_e attribute ) const
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

void monk_t::invalidate_cache( cache_e cache )
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

void monk_t::copy_from( player_t* source )
{
  // TODO: INACTIVE
  player_t::copy_from( source );
  // monk_t* source_p = debug_cast<monk_t*>( source );

  // user_options = source_p->user_options;
}

void monk_t::create_options()
{
  player_t::create_options();

  // add_option( opt_int( "monk.initial_chi", user_options.initial_chi, 0, 6 ) );
  // add_option( opt_float( "monk.expel_harm_effectiveness", user_options.expel_harm_effectiveness, 0.0, 1.0 ) );
  // add_option( opt_float( "monk.faeline_stomp_uptime", user_options.faeline_stomp_uptime, 0.0, 1.0 ) );
  // add_option( opt_int( "monk.chi_burst_healing_targets", user_options.chi_burst_healing_targets, 0, 30 ) );
  // add_option( opt_int( "monk.motc_override", user_options.motc_override, 0, 5 ) );
  // add_option( opt_float( "monk.squirm_frequency", user_options.squirm_frequency, 0, 30 ) );
}

resource_e monk_t::primary_resource() const
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

stat_e monk_t::convert_hybrid_stat( stat_e stat ) const
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

std::string monk_t::default_flask() const
{
  return monk_apl::flask( this );
}

std::string monk_t::default_potion() const
{
  return monk_apl::potion( this );
}

std::string monk_t::default_food() const
{
  return monk_apl::food( this );
}

std::string monk_t::default_rune() const
{
  return monk_apl::rune( this );
}

std::string monk_t::default_temporary_enchant() const
{
  return monk_apl::temporary_enchant( this );
}

monk_td_t::monk_td_t( player_t* target, monk_t* player )
  : actor_target_data_t( target, player ), dot(), debuff(), player( player )
{
}

template <class Base>
monk_action_t<Base>::monk_action_t( std::string_view name, monk_t* player,
                                    const spell_data_t* spell_data /*= spell_data_t::nil()*/ )
  : Base( name, player, spell_data ), parse_buff_effects_t( this )
{
  if ( Base::data().ok() )
  {
    apply_buff_effects();
    apply_debuff_effects();
  }
}

template <class Base>
void monk_action_t<Base>::apply_buff_effects()
{
}

template <class Base>
void monk_action_t<Base>::apply_debuff_effects()
{
}

template <class Base>
monk_t* monk_action_t<Base>::player()
{
  return debug_cast<monk_t*>( Base::player );
}

template <class Base>
const monk_t* monk_action_t<Base>::player() const
{
  return debug_cast<monk_t*>( Base::player );
}

monk_attack_t::monk_attack_t( std::string_view name, monk_t* player,
                              const spell_data_t* spell /*= spell_data_t::nil()*/ )
  : base_t( name, player, spell )
{
  special        = true;
  may_glance     = false;
  track_cd_waste = true;
}

action_t* monk_t::create_action( std::string_view name, std::string_view options_str )
{
  // Baseline Abilities
  if ( name == "blackout_kick" )
    return new blackout_kick_t( this, options_str );
  if ( name == "auto_attack" )
    return new auto_attack_t( this, options_str );

  return player_t::create_action( name, options_str );
}

monk_report_t::bug_t::bug_t( std::string_view description, std::string_view date, bool match )
  : description( description ), date( date ), match( match )
{
}

monk_report_t::monk_report_t( monk_t& player ) : player( player )
{
}

void monk_report_t::bug_report( report::sc_html_stream& os )
{
  auto ReportIssue = [ this ]( std::string_view description, std::string_view date, bool match = false ) {
    issues.push_back( new bug_t( description, date, match ) );
  };

  // ReportIssue( "Issue Description", "Date", Implemented? );
  ReportIssue( "Issue Description", "Date", true );

  os << "<div class=\"section\">\n";
  os << "<h2 class=\"toggle\">Known Bugs and Issues</h2>\n";
  os << "<div class=\"toggle-content hide\">\n";

  for ( bug_t* issue : issues )
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

void monk_report_t::html_customsection( report::sc_html_stream& os )
{
  bug_report( os );

  (void)player;
}

monk_module_t::monk_module_t() : module_t( MONK )
{
}

player_t* monk_module_t::create_player( sim_t* sim, std::string_view name, race_e race ) const
{
  monk_t* player           = new monk_t( sim, name, race );
  player->report_extension = std::make_unique<monk_report_t>( *player );

  return player;
}

bool monk_module_t::valid() const
{
  return true;
}

void monk_module_t::static_init() const
{
}

void monk_module_t::register_hotfixes() const
{
}

void monk_module_t::init( player_t* ) const
{
}

void monk_module_t::combat_begin( sim_t* ) const
{
}

void monk_module_t::combat_end( sim_t* ) const
{
}
}  // namespace monk

const module_t* module_t::monk()
{
  static monk::monk_module_t monk;

  return &monk;
}
