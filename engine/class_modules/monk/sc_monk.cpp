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

namespace monk
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
    struct general_t
    {
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
    struct general_t
    {
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
  } passive;

  // Talents
  struct talent_t
  {
    struct general_t
    {
      // Row 1
      player_talent_t soothing_mist;
      player_talent_t rising_sun_kick;
      player_talent_t tigers_lust;

      // Row 2
      player_talent_t improved_roll;
      player_talent_t calming_presence;
      player_talent_t disable;

      // Row 3
      player_talent_t tiger_tail_sweep;
      player_talent_t vigorous_expulsion;
      player_talent_t improved_vivify;
      player_talent_t detox;           // brewmaster + windwalker
      player_talent_t improved_detox;  // mistweaver

      // Row 4
      // Row 5
      // Row 6
      // Row 7
      // Row 8
      // Row 9
      // Row 10
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
  } talent;

  // Masteries
  struct mastery_t
  {
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

  // void merge( player_t& other )
  // {
  //   // TODO: STUB
  //   player_t::merge( other );
  //   const monk_t& other_monk = static_cast<const monk_t&>( other );
  // }

  // role_e primary_role() const
  // {
  //   // TODO: INACTIVE
  //   switch ( player_t::primary_role() )
  //   {
  //     case ROLE_NONE:
  //     case ROLE_ATTACK:
  //     case ROLE_SPELL:
  //     case ROLE_HYBRID:
  //     case ROLE_DPS:
  //     case ROLE_TANK:
  //     case ROLE_HEAL:
  //       return player_t::primary_role();
  //     case ROLE_MAX:
  //     default:
  //       break;
  //   }

  //   switch ( specialization() )
  //   {
  //     case MONK_BREWMASTER:
  //       return ROLE_TANK;
  //     case MONK_MISTWEAVER:
  //     case MONK_WINDWALKER:
  //     default:
  //       return ROLE_ATTACK;
  //   }
  // }

  action_t* create_action( std::string_view /* name */, std::string_view /* options_str */ )
  {
    // TODO: STUB
    // auto eval = [ name ]( std::string_view compare_name ) { return name == compare_name; };

    // if ( eval("snapshot_stats") ) return new action_t::action_t();
  }

  void init_spells()
  {
    // TODO: STUB
    player_t::init_spells();
  }

  void init_scaling()
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

  void init_rng()
  {
    // TODO: STUB
    player_t::init_rng();
  }

  void reset()
  {
    // TODO: STUB
    player_t::reset();

    if ( sim->debug )
    {
      // std::ostream* stream = sim->out_debug.raw().get_stream();
    }
  }

  double matching_gear_multiplier( attribute_e /*attribute*/ ) const
  {
    // TODO: VERIFY SC
    // TODO: STUB

    // switch ( attribute )
    // {
    //   case ATTR_AGILITY:
    //   case ATTR_INTELLECT:
    //   case ATTR_STAMINA:
    //     return spec.leather_specialization->effectN( index ).perfect();
    //   default:
    //     return 0.0;
    // }
    return 0.0;
  }

  void invalidate_cache( cache_e cache )
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

  void copy_from( player_t* source )
  {
    // TODO: INACTIVE
    player_t::copy_from( source );
    // monk_t* source_p = debug_cast<monk_t*>( source );

    // user_options = source_p->user_options;
  }

  void create_options()
  {
    player_t::create_options();

    // add_option( opt_int( "monk.initial_chi", user_options.initial_chi, 0, 6 ) );
    // add_option( opt_float( "monk.expel_harm_effectiveness", user_options.expel_harm_effectiveness, 0.0, 1.0 ) );
    // add_option( opt_float( "monk.faeline_stomp_uptime", user_options.faeline_stomp_uptime, 0.0, 1.0 ) );
    // add_option( opt_int( "monk.chi_burst_healing_targets", user_options.chi_burst_healing_targets, 0, 30 ) );
    // add_option( opt_int( "monk.motc_override", user_options.motc_override, 0, 5 ) );
    // add_option( opt_float( "monk.squirm_frequency", user_options.squirm_frequency, 0, 30 ) );
  }

  resource_e primary_resource() const
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

  stat_e convert_hybrid_stat( stat_e stat ) const
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

  std::string default_flask() const
  {
    return monk_apl::flask( this );
  }

  std::string default_potion() const
  {
    return monk_apl::potion( this );
  }

  std::string default_food() const
  {
    return monk_apl::food( this );
  }

  std::string default_rune() const
  {
    return monk_apl::rune( this );
  }

  std::string default_temporary_enchant() const
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
    : Base( name, player, spell_data )
  {
  }
};
}  // namespace monk

namespace monk
{
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
}  // namespace monk

const module_t* module_t::monk()
{
  static monk::monk_module_t m;

  return &m;
}
