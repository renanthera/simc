// ==========================================================================
// Dedmonwakeen's DPS-DPM Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include "config.hpp"
#include "simulationcraft.hpp"
#include "class_modules/apl/apl_evoker.hpp"

namespace
{
// ==========================================================================
// Evoker
// ==========================================================================

// Forward declarations
struct evoker_t;

enum empower_e
{
  EMPOWER_NONE = 0,
  EMPOWER_1 = 1,
  EMPOWER_2,
  EMPOWER_3,
  EMPOWER_4,
  EMPOWER_MAX
};

enum spell_color_e
{
  SPELL_COLOR_NONE = 0,
  SPELL_BLACK,
  SPELL_BLUE,
  SPELL_BRONZE,
  SPELL_GREEN,
  SPELL_RED
};

struct empowered_state_t : public action_state_t
{
  empower_e empower;

  empowered_state_t( action_t* a, player_t* t ) : action_state_t( a, t ), empower( empower_e::EMPOWER_NONE ) {}

  void initialize() override
  {
    action_state_t::initialize();
    empower = empower_e::EMPOWER_NONE;
  }

  void copy_state( const action_state_t* s ) override
  {
    action_state_t::copy_state( s );
    empower = debug_cast<const empowered_state_t*>( s )->empower;
  }

  std::ostringstream& debug_str( std::ostringstream& s ) override
  {
    action_state_t::debug_str( s ) << " empower_level=" << static_cast<int>( empower );
    return s;
  }
};

struct evoker_td_t : public actor_target_data_t
{
  struct dots_t
  {
    dot_t* fire_breath;
    dot_t* disintegrate;
  } dots;

  struct debuffs_t
  {
    buff_t* shattering_star;
  } debuffs;

  evoker_td_t( player_t* target, evoker_t* source );
};

struct evoker_t : public player_t
{
  // !!!===========================================================================!!!
  // !!! Runtime variables NOTE: these MUST be properly reset in evoker_t::reset() !!!
  // !!!===========================================================================!!!

  // !!!===========================================================================!!!

  // Options
  struct options_t
  {

  } option;

  // Action pointers
  struct actions_t
  {
    action_t* volatility;
  } action;

  // Buffs
  struct buffs_t
  {
    // Baseline Abilities
    propagate_const<buff_t*> essence_burst;
    propagate_const<buff_t*> hover;
    propagate_const<buff_t*> leaping_flames;
    propagate_const<buff_t*> tailwind;

    // Class Traits
    propagate_const<buff_t*> ancient_flame;
    propagate_const<buff_t*> obsidian_scales;
    propagate_const<buff_t*> scarlet_adaptation;
    propagate_const<buff_t*> tip_the_scales;

    // Devastation Traits
    propagate_const<buff_t*> burnout;
    propagate_const<buff_t*> charged_blast;
    propagate_const<buff_t*> dragonrage;
    propagate_const<buff_t*> iridescence_blue;
    propagate_const<buff_t*> iridescence_red;
    propagate_const<buff_t*> power_swell;
    propagate_const<buff_t*> snapfire;
    // Preservation Traits
  } buff;

  // Specialization Spell Data
  struct specializations_t
  {
    // Baseline
    const spell_data_t* evoker;        // evoker class aura
    const spell_data_t* devastation;   // devastation class aura
    const spell_data_t* preservation;  // preservation class aura

    const spell_data_t* living_flame_damage;
    const spell_data_t* living_flame_heal;
    // Devastation

    // Preservation
  } spec;

  // Talents
  struct talents_t
  {
    // Class Traits
    player_talent_t landslide;  // row 1
    player_talent_t obsidian_scales;
    player_talent_t expunge;
    player_talent_t natural_convergence;  // row 2
    player_talent_t permeating_chill;
    player_talent_t verdant_embrace;
    player_talent_t forger_of_mountains;  // row 3
    player_talent_t innate_magic;
    player_talent_t obsidian_bulwark;
    player_talent_t enkindled;
    player_talent_t scarlet_adaptation;
    player_talent_t quell;  // row 4
    player_talent_t recall;
    player_talent_t heavy_wingbeats;
    player_talent_t clobbering_sweep;
    player_talent_t tailwind;
    player_talent_t cauterizing_flame;
    player_talent_t roar_of_exhilaration;  // row 5
    player_talent_t instinctive_arcana;
    player_talent_t tip_the_scales;
    player_talent_t attuned_to_the_dream;
    player_talent_t sleep_walk;
    player_talent_t draconic_legacy;  // row 6
    player_talent_t tempered_scales;
    player_talent_t extended_flight;
    player_talent_t bountiful_bloom;
    player_talent_t blast_furnace;  // row 7
    player_talent_t exuberance;
    player_talent_t source_of_magic;
    player_talent_t ancient_flame;
    player_talent_t unravel;  // row 8
    player_talent_t protracted_talons;
    player_talent_t oppressing_roar;
    player_talent_t rescue;
    player_talent_t lush_growth;
    player_talent_t renewing_blaze;
    player_talent_t leaping_flames;  // row 9
    player_talent_t overawe;
    player_talent_t aerial_mastery;
    player_talent_t twin_guardian;
    player_talent_t foci_of_life;
    player_talent_t fire_within;
    player_talent_t terror_of_the_skies;  // row 10
    player_talent_t time_spiral;
    player_talent_t zephyr;

    // Devastation Traits
    player_talent_t pyre;  // row 1
    player_talent_t ruby_essence_burst;  // row 2
    player_talent_t azure_essence_burst;
    player_talent_t dense_energy;  // row 3
    player_talent_t imposing_presence;
    player_talent_t inner_radiance;
    player_talent_t eternity_surge;
    player_talent_t volatility;  // row 4
    player_talent_t power_nexus;
    player_talent_t dragonrage;
    player_talent_t lay_waste;
    player_talent_t arcane_intensity;
    player_talent_t ruby_embers;  // row 5
    player_talent_t engulfing_blaze;
    player_talent_t animosity;
    player_talent_t essence_attunement;
    player_talent_t firestorm;  // row 6
    player_talent_t heat_wave;
    player_talent_t titanic_wrath;
    player_talent_t honed_aggression;
    player_talent_t eternitys_span;
    player_talent_t eye_of_infinity;
    player_talent_t casuality;
    player_talent_t catalyze;  // row 7
    player_talent_t tyranny;
    player_talent_t charged_blast;
    player_talent_t shattering_star;
    player_talent_t snapfire;  // row 8
    player_talent_t font_of_magic;
    player_talent_t onyx_legacy;
    player_talent_t spellweavers_dominance;
    player_talent_t focusing_iris;
    player_talent_t arcane_vigor;
    player_talent_t burnout;  // row 9
    player_talent_t imminent_destruction;
    player_talent_t scintillation;
    player_talent_t power_swell;
    player_talent_t feed_the_flames;  // row 10
    player_talent_t everburning_flame;
    player_talent_t cascading_power;
    player_talent_t iridescence;

    // Preservation Traits
  } talent;

  // Benefits
  struct benefits_t
  {

  } benefit;

  // Cooldowns
  struct cooldowns_t
  {
    propagate_const<cooldown_t*> eternity_surge;
    propagate_const<cooldown_t*> fire_breath;
    propagate_const<cooldown_t*> firestorm;
  } cooldown;

  // Gains
  struct gains_t
  {
    propagate_const<gain_t*> roar_of_exhilaration;
  } gain;

  // Procs
  struct procs_t
  {
    propagate_const<proc_t*> ruby_essence_burst;
    propagate_const<proc_t*> azure_essence_burst;
    propagate_const<proc_t*> eye_of_infinity;
  } proc;

  // RPPMs
  struct rppms_t
  {

  } rppm;

  // Uptimes
  struct uptimes_t
  {

  } uptime;

  evoker_t( sim_t * sim, std::string_view name, race_e r = RACE_DRACTHYR_HORDE );

  // Character Definitions
  void init_action_list() override;
  void init_base_stats() override;
  //void init_resources( bool ) override;
  //void init_benefits() override;
  void init_gains() override;
  void init_procs() override;
  //void init_rng() override;
  //void init_uptimes() override;
  void init_spells() override;
  //void init_finished() override;
  void create_actions() override;
  void create_buffs() override;
  void create_options() override;
  //void arise() override;
  //void combat_begin() override;
  //void combat_end() override;
  void analyze( sim_t& ) override;
  void reset() override;
  void copy_from( player_t* ) override;
  void merge( player_t& ) override;
  std::string default_potion() const override;
  std::string default_flask() const override;
  std::string default_food() const override;
  std::string default_rune() const override;
  std::string default_temporary_enchant() const override;

  target_specific_t<evoker_td_t> target_data;
  const evoker_td_t* find_target_data( const player_t* target ) const override;
  evoker_td_t* get_target_data( player_t* target ) const override;

  void apply_affecting_auras( action_t& action ) override;
  action_t* create_action( std::string_view name, std::string_view options_str ) override;
  std::unique_ptr<expr_t> create_expression( std::string_view expr_str ) override;

  // Stat & Multiplier overrides
  double composite_armor() const override;
  double composite_attribute_multiplier( attribute_e ) const override;
  double composite_player_multiplier( school_e ) const override;
  stat_e convert_hybrid_stat( stat_e ) const override;
  double passive_movement_modifier() const override;
  double resource_regen_per_second( resource_e ) const override;
  void target_mitigation( school_e, result_amount_type, action_state_t* ) override;
  double temporary_movement_modifier() const override;

  // Utility functions
  const spelleffect_data_t* find_spelleffect( const spell_data_t* spell,
                                              effect_subtype_t subtype,
                                              int misc_value = P_GENERIC,
                                              const spell_data_t* affected = spell_data_t::nil(),
                                              effect_type_t type = E_APPLY_AURA );
  const spell_data_t* find_spell_override( const spell_data_t* base, const spell_data_t* passive );

  std::vector<action_t*> secondary_action_list;

  template <typename T, typename... Ts>
  T* get_secondary_action( std::string_view n, Ts&&... args )
  {
    auto it = range::find( secondary_action_list, n, &action_t::name_str );
    if ( it != secondary_action_list.cend() )
      return dynamic_cast<T*>( *it );

    auto a = new T( this, std::forward<Ts>( args )... );
    a->background = true;
    secondary_action_list.push_back( a );
    return a;
  }
};

namespace buffs
{
// Template for base evoker buffs
template <class Base>
struct evoker_buff_t : public Base
{
private:
  using bb = Base;  // buff base, buff_t/stat_buff_t/etc.

protected:
  using base_t = evoker_buff_t<buff_t>;  // shorthand

public:
  evoker_buff_t( evoker_t* player, std::string_view name, const spell_data_t* spell = spell_data_t::nil(),
                 const item_t* item = nullptr )
    : bb( player, name, spell, item )
  {}

  evoker_buff_t( evoker_td_t& td, std::string_view name, const spell_data_t* spell = spell_data_t::nil(),
                 const item_t* item = nullptr )
    : bb( td, name, spell, item )
  {}

  evoker_t* p()
  { return static_cast<evoker_t*>( bb::source ); }

  const evoker_t* p() const
  { return static_cast<evoker_t*>( bb::source ); }
};
}  // namespace buffs

// Template for base evoker action code.
template <class Base>
struct evoker_action_t : public Base
{
private:
  using ab = Base;  // action base, spell_t/heal_t/etc.

public:
  // auto parsed dynamic effects
  using bfun = std::function<bool()>;
  struct buff_effect_t
  {
    buff_t* buff;
    double value;
    bool use_stacks;
    bfun func;

    buff_effect_t( buff_t* b, double v, bool s = true, bfun f = nullptr )
      : buff( b ), value( v ), use_stacks( s ), func( std::move( f ) )
    {}
  };

  using dfun = std::function<buff_t*( evoker_td_t* )>;
  struct debuff_effect_t
  {
    dfun func;
    double value;
    bool use_stacks;

    debuff_effect_t( dfun f, double v, bool b )
      : func( std::move( f ) ), value( v ), use_stacks( b )
    {}
  };

  std::vector<buff_effect_t> ta_multiplier_buffeffects;
  std::vector<buff_effect_t> da_multiplier_buffeffects;
  std::vector<buff_effect_t> execute_time_buffeffects;
  std::vector<buff_effect_t> dot_duration_buffeffects;
  std::vector<buff_effect_t> recharge_multiplier_buffeffects;
  std::vector<buff_effect_t> cost_buffeffects;
  std::vector<buff_effect_t> crit_chance_buffeffects;
  std::vector<debuff_effect_t> target_multiplier_debuffeffects;

  spell_color_e spell_color;
  bool move_during_hover;

  evoker_action_t( std::string_view name, evoker_t* player, const spell_data_t* spell = spell_data_t::nil() )
    : ab( name, player, spell ), spell_color( SPELL_COLOR_NONE ), move_during_hover( false )
  {
    // TODO: find out if there is a better data source for the spell color
    if ( ab::data().ok() )
    {
      const auto& desc = player->dbc->spell_text( ab::data().id() ).rank();
      if ( desc )
      {
        if ( util::str_compare_ci( desc, "Black" ) )
          spell_color = SPELL_BLACK;
        else if ( util::str_compare_ci( desc, "Blue" ) )
          spell_color = SPELL_BLUE;
        else if ( util::str_compare_ci( desc, "Bronze" ) )
          spell_color = SPELL_BRONZE;
        else if ( util::str_compare_ci( desc, "Green" ) )
          spell_color = SPELL_GREEN;
        else if ( util::str_compare_ci( desc, "Red" ) )
          spell_color = SPELL_RED;
      }

      apply_buff_effects();
      apply_debuffs_effects();

      move_during_hover =
          player->find_spelleffect( player->find_class_spell( "Hover" ), A_CAST_WHILE_MOVING_WHITELIST, 0, &ab::data() )
              ->ok();
    }
  }

  evoker_t* p()
  { return static_cast<evoker_t*>( ab::player ); }

  const evoker_t* p() const
  { return static_cast<evoker_t*>( ab::player ); }

  evoker_td_t* td( player_t* t ) const
  { return p()->get_target_data( t ); }

  const evoker_td_t* find_td( const player_t* t ) const
  { return p()->find_target_data( t ); }

  bool usable_moving() const override
  { return move_during_hover && p()->buff.hover->check(); }

  template <typename T>
  void parse_spell_effects_mods( double& val, const spell_data_t* base, size_t idx, T mod )
  {
    for ( size_t i = 1; i <= mod->effect_count(); i++ )
    {
      const auto& eff = mod->effectN( i );

      if ( eff.type() != E_APPLY_AURA )
        continue;

      if ( ( base->affected_by_all( eff ) &&
             ( ( eff.misc_value1() == P_EFFECT_1 && idx == 1 ) || ( eff.misc_value1() == P_EFFECT_2 && idx == 2 ) ||
               ( eff.misc_value1() == P_EFFECT_3 && idx == 3 ) || ( eff.misc_value1() == P_EFFECT_4 && idx == 4 ) ||
               ( eff.misc_value1() == P_EFFECT_5 && idx == 5 ) ) ) ||
           ( eff.subtype() == A_PROC_TRIGGER_SPELL_WITH_VALUE && eff.trigger_spell_id() == base->id() && idx == 1 ) )
      {
        double pct = eff.percent();

        if ( eff.subtype() == A_ADD_FLAT_MODIFIER || eff.subtype() == A_ADD_FLAT_LABEL_MODIFIER )
          val += pct;
        else if ( eff.subtype() == A_ADD_PCT_MODIFIER || eff.subtype() == A_ADD_PCT_LABEL_MODIFIER )
          val *= 1.0 + pct;
        else if ( eff.subtype() == A_PROC_TRIGGER_SPELL_WITH_VALUE )
          val = pct;
        else
          continue;
      }
    }
  }

  void parse_spell_effects_mods( double&, const spell_data_t*, size_t ) {}

  template <typename T, typename... Ts>
  void parse_spell_effects_mods( double& val, const spell_data_t* base, size_t idx, T mod, Ts... mods )
  {
    parse_spell_effects_mods( val, base, idx, mod );
    parse_spell_effects_mods( val, base, idx, mods... );
  }

  // Will parse simple buffs that ONLY target the caster and DO NOT have multiple ranks
  // 1: Add Percent Modifier to Spell Direct Amount
  // 2: Add Percent Modifier to Spell Periodic Amount
  // 3: Add Percent Modifier to Spell Cast Time
  // 4: Add Percent Modifier to Spell Cooldown
  // 5: Add Percent Modifier to Spell Resource Cost
  // 6: Add Flat Modifier to Spell Critical Chance
  template <typename... Ts>
  void parse_buff_effect( buff_t* buff, bfun f, const spell_data_t* s_data, size_t i, bool use_stacks, bool use_default,
                          Ts... mods )
  {
    const auto& eff = s_data->effectN( i );
    double val      = eff.percent();

    auto debug_message = [ & ]( std::string_view type ) {
      if ( buff )
      {
        p()->sim->print_debug( "buff-effects: {} ({}) {} modified by {}% with buff {} ({}#{})", ab::name(), ab::id,
                               type, val * 100.0, buff->name(), buff->data().id(), i );
      }
      else if ( f )
      {
        p()->sim->print_debug( "conditional-effects: {} ({}) {} modified by {}% with condition from {} ({}#{})",
                               ab::name(), ab::id, type, val * 100.0, s_data->name_cstr(), s_data->id(), i );
      }
      else
      {
        p()->sim->print_debug( "passive-effects: {} ({}) {} modified by {}% from {} ({}#{})", ab::name(), ab::id, type,
                               val * 100.0, s_data->name_cstr(), s_data->id(), i );
      }
    };

    // TODO: more robust logic around 'party' buffs with radius
    if ( !( eff.type() == E_APPLY_AURA || eff.type() == E_APPLY_AREA_AURA_PARTY ) || eff.radius() ) return;

    if ( i <= 5 )
      parse_spell_effects_mods( val, s_data, i, mods... );

    if ( !ab::data().affected_by_all( eff ) )
      return;

    if ( use_default && buff)
      val = buff->default_value;

    if ( !val )
      return;

    if ( eff.subtype() == A_ADD_PCT_MODIFIER || eff.subtype() == A_ADD_PCT_LABEL_MODIFIER )
    {
      switch ( eff.misc_value1() )
      {
        case P_GENERIC:
          da_multiplier_buffeffects.emplace_back( buff, val, use_stacks, f );
          debug_message( "direct damage" );
          break;
        case P_DURATION:
          dot_duration_buffeffects.emplace_back( buff, val, use_stacks, f );
          debug_message( "duration" );
          break;
        case P_TICK_DAMAGE:
          ta_multiplier_buffeffects.emplace_back( buff, val, use_stacks, f );
          debug_message( "tick damage" );
          break;
        case P_CAST_TIME:
          execute_time_buffeffects.emplace_back( buff, val, use_stacks, f );
          debug_message( "cast time" );
          break;
        case P_COOLDOWN:
          recharge_multiplier_buffeffects.emplace_back( buff, val, use_stacks, f );
          debug_message( "cooldown" );
          break;
        case P_RESOURCE_COST:
          cost_buffeffects.emplace_back( buff, val, use_stacks, f );
          debug_message( "cost" );
          break;
        default:
          return;
      }
    }
    else if ( eff.subtype() == A_ADD_FLAT_MODIFIER && eff.misc_value1() == P_CRIT )
    {
      crit_chance_buffeffects.emplace_back( buff, val, use_stacks, f );
      debug_message( "crit chance" );
    }
    else
    {
      return;
    }
  }

  template <typename... Ts>
  void parse_buff_effects( buff_t* buff, unsigned ignore_mask, bool use_stacks, bool use_default, Ts... mods )
  {
    if ( !buff )
      return;

    const spell_data_t* s_data = &buff->data();
    for ( size_t i = 1; i <= s_data->effect_count(); i++ )
    {
      if ( ignore_mask & 1 << ( i - 1 ) )
        continue;

      parse_buff_effect( buff, nullptr, s_data, i, use_stacks, use_default, mods... );
    }
  }

  template <typename... Ts>
  void parse_buff_effects( buff_t* buff, unsigned ignore_mask, Ts... mods )
  { parse_buff_effects<Ts...>( buff, ignore_mask, true, false, mods... ); }

  template <typename... Ts>
  void parse_buff_effects( buff_t* buff, bool stack, bool use_default, Ts... mods )
  { parse_buff_effects<Ts...>( buff, 0U, stack, use_default, mods... ); }

  template <typename... Ts>
  void parse_buff_effects( buff_t* buff, bool stack, Ts... mods )
  { parse_buff_effects<Ts...>( buff, 0U, stack, false, mods... ); }

  template <typename... Ts>
  void parse_buff_effects( buff_t* buff, Ts... mods )
  { parse_buff_effects<Ts...>( buff, 0U, true, false, mods... ); }

  void parse_conditional_effects( const spell_data_t* spell, bfun f, unsigned ignore_mask = 0U )
  {
    if ( !spell || !spell->ok() )
      return;

    for ( size_t i = 1 ; i <= spell->effect_count(); i++ )
    {
      if ( ignore_mask & 1 << ( i - 1 ) )
        continue;

      parse_buff_effect( nullptr, f, spell, i, false, false );
    }
  }

  void parse_passive_effects( const spell_data_t* spell, unsigned ignore_mask = 0U )
  { parse_conditional_effects( spell, nullptr, ignore_mask ); }

  double get_buff_effects_value( const std::vector<buff_effect_t>& buffeffects, bool flat = false,
                                 bool benefit = true ) const
  {
    double return_value = flat ? 0.0 : 1.0;

    for ( const auto& i : buffeffects )
    {
      double eff_val = i.value;
      int mod = 1;

      if ( i.func && !i.func() )
          continue;  // continue to next effect if conditional effect function is false

      if ( i.buff )
      {
        auto stack = benefit ? i.buff->stack() : i.buff->check();

        if ( !stack )
          continue;  // continue to next effect if stacks == 0 (buff is down)

        mod = i.use_stacks ? stack : 1;
      }

      if ( flat )
        return_value += eff_val * mod;
      else
        return_value *= 1.0 + eff_val * mod;
    }

    return return_value;
  }

  // Syntax: parse_buff_effects[<S[,S...]>]( buff[, ignore_mask|use_stacks[, use_default]][, spell1[,spell2...] )
  //  buff = buff to be checked for to see if effect applies
  //  ignore_mask = optional bitmask to skip effect# n corresponding to the n'th bit
  //  use_stacks = optional, default true, whether to multiply value by stacks
  //  use_default = optional, default false, whether to use buff's default value over effect's value
  //  S = optional list of template parameter(s) to indicate spell(s) with redirect effects
  //  spell = optional list of spell(s) with redirect effects that modify the effects on the buff
  void apply_buff_effects()
  {
    // using S = const spell_data_t*;

    parse_buff_effects( p()->buff.ancient_flame );
    parse_buff_effects( p()->buff.burnout );
    parse_buff_effects( p()->buff.essence_burst );
    parse_buff_effects( p()->buff.snapfire );
    parse_buff_effects( p()->buff.tip_the_scales );
  }

  template <typename... Ts>
  void parse_debuff_effects( const dfun& func, bool use_stacks, const spell_data_t* s_data, Ts... mods )
  {
    if ( !s_data->ok() )
      return;

    for ( size_t i = 1; i <= s_data->effect_count(); i++ )
    {
      const auto& eff = s_data->effectN( i );
      double val      = eff.percent();

      if ( eff.type() != E_APPLY_AURA )
        continue;

      if ( eff.subtype() != A_MOD_DAMAGE_FROM_CASTER_SPELLS || !ab::data().affected_by_all( eff ) )
        continue;

      if ( i <= 5 )
        parse_spell_effects_mods( val, s_data, i, mods... );

      if ( !val )
        continue;

      p()->sim->print_debug( "debuff-effects: {} ({}) damage modified by {}% on targets with debuff {} ({}#{})", ab::name(),
                             ab::id, val * 100.0, s_data->name_cstr(), s_data->id(), i );
      target_multiplier_debuffeffects.emplace_back( func, val, use_stacks  );
    }
  }

  template <typename... Ts>
  void parse_debuff_effects( dfun func, const spell_data_t* s_data, Ts... mods )
  { parse_debuff_effects( func, true, s_data, mods... ); }

  double get_debuff_effect_values( evoker_td_t* t ) const
  {
    double return_value = 1.0;

    for ( const auto& i : target_multiplier_debuffeffects )
    {
      auto debuff = i.func( t );

      if ( debuff->check() )
        return_value *= 1.0 + i.value * ( i.use_stacks ? debuff->check() : 1.0 );
    }

    return return_value;
  }

  // Syntax: parse_dot_debuffs[<S[,S...]>]( func, spell_data_t* dot[, spell_data_t* spell1[,spell2...] )
  //  func = function returning the dot_t* of the dot
  //  dot = spell data of the dot
  //  S = optional list of template parameter(s) to indicate spell(s)with redirect effects
  //  spell = optional list of spell(s) with redirect effects that modify the effects on the dot
  void apply_debuffs_effects()
  {
    // using S = const spell_data_t*;

    parse_debuff_effects( []( evoker_td_t* t ) -> buff_t* { return t->debuffs.shattering_star; },
        p()->talent.shattering_star );
  }

  double cost() const override
  {
    double c = ab::cost() * std::max( 0.0, get_buff_effects_value( cost_buffeffects, false, false ) );
    return c;
  }

  double composite_target_multiplier( player_t* t ) const override
  {
    double tm = ab::composite_target_multiplier( t ) * get_debuff_effect_values( td( t ) );
    return tm;
  }

  double composite_ta_multiplier( const action_state_t* s ) const override
  {
    double ta = ab::composite_ta_multiplier( s ) * get_buff_effects_value( ta_multiplier_buffeffects );
    return ta;
  }

  double composite_da_multiplier( const action_state_t* s ) const override
  {
    double da = ab::composite_da_multiplier( s ) * get_buff_effects_value( da_multiplier_buffeffects );
    return da;
  }

  double composite_crit_chance() const override
  {
    double cc = ab::composite_crit_chance() + get_buff_effects_value( crit_chance_buffeffects, true );
    return cc;
  }

  timespan_t execute_time() const override
  {
    timespan_t et = ab::execute_time() * get_buff_effects_value( execute_time_buffeffects );
    return std::max( 0_ms, et );
  }

  timespan_t composite_dot_duration( const action_state_t* s ) const override
  {
    timespan_t dd = ab::composite_dot_duration( s ) * get_buff_effects_value( dot_duration_buffeffects );
    return dd;
  }

  double recharge_multiplier( const cooldown_t& cd ) const override
  {
    double rm = ab::recharge_multiplier( cd ) * get_buff_effects_value( recharge_multiplier_buffeffects, false, false );
    return rm;
  }
};

namespace heals
{
struct evoker_heal_t : public evoker_action_t<heal_t>
{
private:
  using ab = evoker_action_t<heal_t>;

public:
  double scarlet_adaptation_sp_cap;

  evoker_heal_t( std::string_view name, evoker_t* player, const spell_data_t* spell = spell_data_t::nil(),
                 std::string_view options_str = {} )
    : ab( name, player, spell ), scarlet_adaptation_sp_cap( player->spec.living_flame_damage->effectN( 1 ).sp_coeff() )
  {
    parse_options( options_str );
  }

  void assess_damage( result_amount_type rt, action_state_t* s ) override
  {
    ab::assess_damage( rt, s );

    if ( p()->talent.scarlet_adaptation.ok() )
    {
      if ( !p()->buff.scarlet_adaptation->check() )
        p()->buff.scarlet_adaptation->trigger();

      auto& stored = p()->buff.scarlet_adaptation->current_value;
      // TODO: raw_amount for used for testing
      // stored += s->result_amount * p()->talent.scarlet_adaptation->effectN( 1 ).percent();
      stored += s->result_raw * p()->talent.scarlet_adaptation->effectN( 1 ).percent();
      // TODO: confirm if this always matches living flame SP coeff
      stored = std::min( stored, p()->cache.spell_power( SCHOOL_MAX ) * scarlet_adaptation_sp_cap );
    }
  }

  double composite_target_multiplier( player_t* t ) const override
  {
    double tm = ab::composite_target_multiplier( t );

    if ( p()->specialization() == EVOKER_PRESERVATION && t->health_percentage() < p()->health_percentage() )
      tm *= 1.0 + p()->cache.mastery_value();

    return tm;
  }
};
}  // namespace heals

namespace spells
{

// Base Classes =============================================================

struct evoker_spell_t : public evoker_action_t<spell_t>
{
private:
  using ab = evoker_action_t<spell_t>;

public:
  evoker_spell_t( std::string_view name, evoker_t* player, const spell_data_t* spell = spell_data_t::nil(),
                  std::string_view options_str = {} )
    : ab( name, player, spell )
  {
    parse_options( options_str );
  }

  void execute() override
  {
    ab::execute();

    if ( p()->talent.casuality.ok() && current_resource() == RESOURCE_ESSENCE )
    {
      p()->cooldown.eternity_surge->adjust( p()->talent.casuality->effectN( 1 ).trigger()->effectN( 1 ).time_value() );
    }
  }

  virtual void trigger_charged_blast( action_state_t* s )
  {
    if ( spell_color == SPELL_BLUE && has_amount_result() && result_is_hit( s->result ) )
      p()->buff.charged_blast->trigger();
  }

  virtual void trigger_everburning_flame( action_state_t* s )
  {
    if ( s->chain_target == 0 && s->result_amount > 0 && s->result_type == result_amount_type::DMG_DIRECT &&
         spell_color == SPELL_RED )
    {
      if ( p()->talent.everburning_flame.ok() )
      {
        for ( auto t : sim->target_non_sleeping_list )
        {
          td( t )->dots.fire_breath->adjust_duration(
              timespan_t::from_seconds( as<int>( p()->talent.everburning_flame->effectN( 1 ).base_value() ) ),
              0_ms, -1, false );
        }
      }
    }
  }

  void impact( action_state_t* s ) override
  {
    ab::impact( s );

    trigger_charged_blast( s );
    trigger_everburning_flame( s );

    if ( !ab::background && !ab::dual )
    {
      // These happen after any secondary spells are executed, so we schedule as events
      if ( spell_color == SPELL_BLUE )
        make_event( *sim, [ this ]() { p()->buff.iridescence_blue->decrement(); } );
      else if ( spell_color == SPELL_RED )
        make_event( *sim, [ this ]() { p()->buff.iridescence_red->decrement(); } );
    }
  }

  void tick( dot_t* d ) override
  {
    ab::tick( d );

    trigger_charged_blast( d->state );
  }

  double composite_target_multiplier( player_t* t ) const override
  {
    double tm = ab::composite_target_multiplier( t );

    // Preliminary testing shows this is linear with target hp %.
    // TODO: confirm this applies only to all evoker offensive spells
    if ( p()->specialization() == EVOKER_DEVASTATION )
    {
      if ( !p()->buff.dragonrage->check() || !p()->talent.tyranny.ok() )
        tm *= 1.0 + p()->cache.mastery_value() * t->health_percentage() / 100;
      else
        tm *= 1.0 + p()->cache.mastery_value();
    }

    return tm;
  }

  double composite_persistent_multiplier( const action_state_t* s ) const override
  {
    auto mult = ab::composite_persistent_multiplier( s );

    if ( current_resource() == RESOURCE_ESSENCE && p()->talent.titanic_wrath.ok() &&
         p()->buff.essence_burst->check() )
    {
      mult *= 1.0 + p()->talent.titanic_wrath->effectN( 1 ).percent();
    }

    // iridescence blue affects the entire channel for disintegrate
    if ( spell_color == SPELL_BLUE && !background )
      mult *= 1.0 + p()->buff.iridescence_blue->check_value();

    return mult;
  }

  void consume_resource() override
  {
    ab::consume_resource();

    resource_e cr = current_resource();

    if ( cr != RESOURCE_ESSENCE || base_cost() == 0 || proc )
      return;

    if ( p()->buff.essence_burst->up() && !rng().roll( p()->talent.cascading_power->effectN( 1 ).percent() ) )
    {
      p()->buff.essence_burst->decrement();

      if ( p()->talent.feed_the_flames.ok() )
      {
        p()->cooldown.fire_breath->adjust(
            -timespan_t::from_seconds( p()->talent.feed_the_flames->effectN( 1 ).base_value() ) );
      }
    }
  }
};

struct empowered_base_t : public evoker_spell_t
{
  empower_e max_empower;

  empowered_base_t( std::string_view name, evoker_t* p, const spell_data_t* spell, std::string_view options_str = {} )
    : evoker_spell_t( name, p, spell, options_str ),
      max_empower( p->talent.font_of_magic.ok() ? empower_e::EMPOWER_4 : empower_e::EMPOWER_3 )
  {}

  action_state_t* new_state() override
  { return new empowered_state_t( this, target ); }
};

struct empowered_release_spell_t : public empowered_base_t
{
  using base_t = empowered_release_spell_t;

  empowered_release_spell_t( std::string_view name, evoker_t* p, const spell_data_t* spell )
    : empowered_base_t( name, p, spell )
  {
    dual = true;

    // TODO: Confirm this still applies, as of 09/Aug/2022 it would appear to be approximately a 1s gcd after a empower finishes
    trigger_gcd = 1_s;
    gcd_type = gcd_haste_type::NONE;
  }

  empower_e empower_level( const action_state_t* s ) const
  { return debug_cast<const empowered_state_t*>( s )->empower; }

  int empower_value( const action_state_t* s ) const
  { return static_cast<int>( debug_cast<const empowered_state_t*>( s )->empower ); }

  void execute() override
  {
    empowered_base_t::execute();

    if ( background )
      return;

    if ( p()->talent.animosity.ok() )
    {
      p()->buff.dragonrage->extend_duration(p(), p()->talent.animosity->effectN( 1 ).time_value() );
    }

    if ( p()->talent.iridescence.ok() )
    {
      if ( spell_color == SPELL_BLUE )
        p()->buff.iridescence_blue->trigger();
      else if ( spell_color == SPELL_RED )
        p()->buff.iridescence_red->trigger();
    }

    if ( p()->talent.power_swell.ok() )
      p()->buff.power_swell->trigger();
  }
};

struct empowered_charge_spell_t : public empowered_base_t
{
  using base_t = empowered_charge_spell_t;

  action_t* release_spell;
  stats_t* dummy_stat;  // used to hack channel tick time into execute time
  stats_t* orig_stat;
  int empower_to;
  timespan_t base_empower_duration;
  timespan_t lag;

  empowered_charge_spell_t( std::string_view name, evoker_t* p, const spell_data_t* spell,
                            std::string_view options_str )
    : empowered_base_t( name, p, p->find_spell_override( spell, p->talent.font_of_magic ) ),
      release_spell( nullptr ),
      dummy_stat( p->get_stats( "dummy_stat" ) ),
      orig_stat( stats ),
      empower_to( max_empower ),
      base_empower_duration( 0_ms ),
      lag( 0_ms )
  {
    channeled = true;

    // TODO: convert to full empower expression support
    add_option( opt_int( "empower_to", empower_to, EMPOWER_1, EMPOWER_MAX ) );

    parse_options( options_str );

    empower_to = std::min( static_cast<int>(max_empower), empower_to );

    dot_duration = base_tick_time = base_empower_duration =
        base_time_to_empower( static_cast<empower_e>( empower_to ) );

    apply_affecting_aura( p->talent.imminent_destruction );
  }

  template <typename T>
  void create_release_spell( std::string_view n )
  {
    static_assert( std::is_base_of_v<empowered_release_spell_t, T>,
                   "Empowered release spell must be dervied from empowered_release_spell_t." );

    release_spell = p()->get_secondary_action<T>( n );
    release_spell->stats = stats;
    release_spell->background = false;
  }

  timespan_t base_time_to_empower( empower_e emp ) const
  {
    // TODO: confirm these values and determine if they're set values or adjust based on a formula
    // Currently all empowered spells are 2.5s base and 3.25s with empower 4
    switch ( emp )
    {
      case empower_e::EMPOWER_1: return 1000_ms;
      case empower_e::EMPOWER_2: return 1750_ms;
      case empower_e::EMPOWER_3: return 2500_ms;
      case empower_e::EMPOWER_4: return 3250_ms;
      default: break;
    }

    return 0_ms;
  }

  timespan_t max_hold_time() const
  {
    // TODO: confirm if this is affected by duration mods/haste
    return base_time_to_empower( max_empower ) + 2_s;
  }

  timespan_t tick_time( const action_state_t* s ) const override
  {
    // we need to have the tick time match duration.
    // NOTE: composite_dot_duration CANNOT reference parent method as spell_t::composite_dot_duration calls tick_time()
    return composite_dot_duration( s );
  }

  timespan_t base_composite_dot_duration( const action_state_t* s ) const
  {
    return dot_duration * s->haste * get_buff_effects_value( dot_duration_buffeffects );
  }

  timespan_t composite_dot_duration( const action_state_t* s ) const override
  {
    // NOTE: DO NOT reference parent method as spell_t::composite_dot_duration calls tick_time()
    auto dur = base_composite_dot_duration( s );

    // hack so we always have a non-zero duration in order to trigger last_tick()
    if ( dur == 0_ms )
      return 1_ms;

    return dur + lag;
  }

  timespan_t composite_time_to_empower( const action_state_t* s, empower_e emp ) const
  {
    auto base = base_time_to_empower( emp );
    auto mult = base_composite_dot_duration( s ) / base_empower_duration;

    return base * mult;
  }

  empower_e empower_level( const dot_t* d ) const
  {
    auto emp = empower_e::EMPOWER_NONE;

    if ( !d->is_ticking() )
      return emp;

    auto s = d->state;
    auto elapsed = tick_time( s ) - d->time_to_next_full_tick();

    if ( elapsed >= composite_time_to_empower( s, empower_e::EMPOWER_4 ) )
      emp = empower_e::EMPOWER_4;
    else if ( elapsed >= composite_time_to_empower( s, empower_e::EMPOWER_3 ) )
      emp = empower_e::EMPOWER_3;
    else if ( elapsed >= composite_time_to_empower( s, empower_e::EMPOWER_2 ) )
      emp = empower_e::EMPOWER_2;
    else if ( elapsed >= composite_time_to_empower( s, empower_e::EMPOWER_1 ) )
      emp = empower_e::EMPOWER_1;

    return std::min( max_empower, emp );
  }

  void init() override
  {
    empowered_base_t::init();
    assert( release_spell && "Empowered charge spell must have a release spell." );
  }

  void execute() override
  {
    // pre-determine lag here per every execute
    lag = rng().gauss( sim->channel_lag, sim->channel_lag_stddev );

    empowered_base_t::execute();
  }

  void tick( dot_t* d ) override
  {
    // For proper DPET analysis, we need to treat charge spells as non-channel, since channelled spells sum up tick
    // times to get the execute time, but this does not work for fire breath which also has a dot component. Instead we
    // hijack the stat obj during action_t:tick() causing the channel's tick to be recorded onto a throwaway stat obj.
    // We then record the corresponding tick time as execute time onto the original real stat obj. See further notes in
    // evoker_t::analyze().
    stats = dummy_stat;
    empowered_base_t::tick( d );
    stats = orig_stat;

    stats->iteration_total_execute_time += d->time_to_tick();
  }

  void last_tick( dot_t* d ) override
  {
    empowered_base_t::last_tick( d );

    auto emp_state = release_spell->get_state();
    emp_state->target = d->state->target;
    release_spell->snapshot_state( emp_state, release_spell->amount_type( emp_state ) );
    debug_cast<empowered_state_t*>( emp_state )->empower = empower_level( d );

    release_spell->schedule_execute( emp_state );

    p()->buff.tip_the_scales->expire();

    // hack to prevent dot_t::last_tick() from schedule_ready()'ing the player
    d->current_action = release_spell;
    // hack to prevent channel lag being added when player is schedule_ready()'d after the release spell execution
    p()->last_foreground_action = release_spell;
  }
};

// Empowered Spells =========================================================

struct fire_breath_t : public empowered_charge_spell_t
{
  struct fire_breath_damage_t : public empowered_release_spell_t
  {
    fire_breath_damage_t( evoker_t* p ) : base_t( "fire_breath_damage", p, p->find_spell( 357209 ) )
    {
      aoe = -1;  // TODO: actually a cone so we need to model it if possible
    }

    timespan_t composite_dot_duration( const action_state_t* s ) const override
    {
      return base_t::composite_dot_duration( s ) * empower_value( s ) / 3.0;
    }

    void trigger_everburning_flame( action_state_t* ) override
    {
      return;  // flame breath can't extend itself. TODO: confirm if this ever becomes a possiblity.
    }

    void execute() override
    {
      base_t::execute();

      if ( p()->talent.leaping_flames.ok() )
        p()->buff.leaping_flames->trigger( empower_value( execute_state ) );
    }

    timespan_t tick_time( const action_state_t* state ) const override
    {
      timespan_t t = base_t::tick_time( state );

      if ( p()->talent.catalyze.ok() && p()->get_target_data( state->target )->dots.disintegrate->is_ticking() )
      {
        t /= ( 1 + p()->talent.catalyze->effectN( 1 ).percent() );
      }

      return t;
    }

    void tick( dot_t* d ) override
    {
      empowered_release_spell_t::tick( d );

      if ( p()->talent.burnout.ok() )
      {
        p()->buff.burnout->trigger();
        if ( p()->buff.burnout->cooldown->down() )
        {
          // TODO: Confirm next build, as of 27/09/2022 the ICD is triggered on attempt not success
          p()->buff.burnout->cooldown->start();
        }
      }
    }

  };

  fire_breath_t( evoker_t* p, std::string_view options_str )
    : base_t( "fire_breath", p, p->find_class_spell( "Fire Breath" ), options_str )
  {
    create_release_spell<fire_breath_damage_t>( "fire_breath_damage" );
  }
};

struct eternity_surge_t : public empowered_charge_spell_t
{
  struct eternity_surge_damage_t : public empowered_release_spell_t
  {
    eternity_surge_damage_t( evoker_t* p, std::string_view name )
      : base_t( name, p, p->find_spell( 359077 ) )
    {}

    eternity_surge_damage_t( evoker_t* p ) : eternity_surge_damage_t( p, "eternity_surge_damage" )
    {}

    int n_targets() const override
    {
      int n = pre_execute_state ? empower_value( pre_execute_state ) : max_empower;

      n *=  as<int>( 1 + p()->talent.eternitys_span->effectN( 2 ).percent() );

      return n == 1 ? 0 : n;
    }

    void impact( action_state_t* s ) override
    {
      base_t::impact( s );

      if ( p()->talent.eye_of_infinity.ok() && result_is_hit( s->result ) && s->result == RESULT_CRIT )
      {
        p()->buff.essence_burst->trigger();
        p()->proc.eye_of_infinity->occur();
      }
    }
  };

  eternity_surge_t( evoker_t* p, std::string_view options_str )
    : base_t( "eternity_surge", p, p->talent.eternity_surge, options_str )
  {
    create_release_spell<eternity_surge_damage_t>( "eternity_surge_damage" );
  }
};

// Spells ===================================================================

struct azure_strike_t : public evoker_spell_t
{
  azure_strike_t( evoker_t* p, std::string_view options_str )
    : evoker_spell_t( "azure_strike", p, p->find_class_spell( "Azure Strike" ), options_str )
  {
    aoe = as<int>( data().effectN( 1 ).base_value() + p->talent.protracted_talons->effectN( 1 ).base_value() );
  }

  void execute() override
  {
    evoker_spell_t::execute();

    if ( p()->talent.azure_essence_burst.ok() &&
         ( p()->buff.dragonrage->up() || rng().roll( p()->talent.azure_essence_burst->effectN( 1 ).percent() ) ) )
    {
      p()->buff.essence_burst->trigger();
      p()->proc.azure_essence_burst->occur();
    }
  }
};

struct deep_breath_t : public evoker_spell_t
{
  struct deep_breath_dot_t : public evoker_spell_t
  {
    deep_breath_dot_t( evoker_t* p ) : evoker_spell_t( "deep_breath_dot", p, p->find_spell( 353759 ) )
    {
      travel_delay = 0.9;   // guesstimate, TODO: confirm
      travel_speed = 19.5;  // guesstimate, TODO: confirm
      aoe = -1;
    }

    timespan_t travel_time() const override
    {
      // we set the execute_time of the base action as travel_time of the damage action so they match, but when we
      // actually execute the damage action, we want travel_time to be 0_ms since it is already accounted for in the
      // base action.
      return execute_state && execute_state->target ? 0_ms : evoker_spell_t::travel_time();
    }
  };

  action_t* damage;

  deep_breath_t( evoker_t* p, std::string_view options_str )
    : evoker_spell_t( "deep_breath", p, p->find_class_spell( "Deep Breath" ) )
  {

    damage = p->get_secondary_action<deep_breath_dot_t>( "deep_breath_dot" );
    damage->stats = stats;
  }

  timespan_t execute_time() const override
  {
    return damage->travel_time();
  }

  void execute() override
  {
    evoker_spell_t::execute();

    damage->execute_on_target( target );
  }
};

struct disintegrate_t : public evoker_spell_t
{
  action_t* eternity_surge;

  disintegrate_t( evoker_t* p, std::string_view options_str )
    : evoker_spell_t( "disintegrate", p, p->find_class_spell( "Disintegrate" ), options_str )
  {
    channeled      = true;
    
    eternity_surge = p->get_secondary_action<eternity_surge_t::eternity_surge_damage_t>( "scintillation", "scintillation" );
    eternity_surge->s_data_reporting = p->talent.scintillation;
    eternity_surge->name_str_reporting = "scintillation";
    
    tick_zero = true;

    add_child( eternity_surge );
  }

  void tick( dot_t* d ) override
  {
    evoker_spell_t::tick( d );

    if ( p()->talent.scintillation.ok() && rng().roll( p()->talent.scintillation->effectN( 2 ).percent() ) )
    {
      auto emp_state    = eternity_surge->get_state();
      emp_state->target = d->state->target;
      eternity_surge->snapshot_state( emp_state, eternity_surge->amount_type( emp_state ) );
      emp_state->persistent_multiplier *= p()->talent.scintillation->effectN( 1 ).percent();
      debug_cast<empowered_state_t*>( emp_state )->empower = EMPOWER_1;

      eternity_surge->schedule_execute( emp_state );
    }
  }
};

struct expunge_t : public evoker_spell_t
{
  expunge_t( evoker_t* p, std::string_view options_str )
    : evoker_spell_t( "expunge", p, p->talent.expunge, options_str )
  {}
};

struct firestorm_t : public evoker_spell_t
{
  struct firestorm_tick_t : public evoker_spell_t
  {
    firestorm_tick_t( evoker_t* p ) : evoker_spell_t( "firestorm_tick", p, p->find_spell( 369374 ) )
    {
      dual = ground_aoe = true;
      aoe = -1;
    }

    double composite_persistent_multiplier( const action_state_t* s ) const override
    {
      auto pm = evoker_spell_t::composite_persistent_multiplier( s );

      pm *= 1.0 + p()->buff.snapfire->check_value();
      pm *= 1.0 + p()->buff.iridescence_red->check_value();

      return pm;
    }
  };

  action_t* damage;

  firestorm_t( evoker_t* p, std::string_view options_str )
    : evoker_spell_t( "firestorm", p, p->talent.firestorm, options_str ),
      damage( p->get_secondary_action<firestorm_tick_t>( "firestorm_tick" ) )
  {
    damage->stats = stats;
  }

  void impact( action_state_t* s ) override
  {
    evoker_spell_t::impact( s );

    make_event<ground_aoe_event_t>( *sim, p(), ground_aoe_params_t()
      .target( s->target)
      .pulse_time( 2_s )  // from text description, not in spell data. TODO: confirm
      .duration( data().effectN( 1 ).trigger()->duration() )
      .action( damage ), true );

    p()->buff.snapfire->expire();
  }
};

struct hover_t : public evoker_spell_t
{
  hover_t( evoker_t* p, std::string_view options_str )
    : evoker_spell_t( "hover", p, p->find_class_spell( "Hover" ), options_str )
  {}

  void execute() override
  {
    evoker_spell_t::execute();

    p()->buff.hover->trigger();
  }
};

struct landslide_t : public evoker_spell_t
{
  landslide_t( evoker_t* p, std::string_view options_str )
    : evoker_spell_t( "landslide", p, p->talent.landslide, options_str )
  {
    // TODO: root NYI
  }
};

struct living_flame_t : public evoker_spell_t
{
  template <class Base>
  struct living_flame_base_t : public Base
  {
    using base_t = living_flame_base_t<Base>;

    living_flame_base_t( std::string_view n, evoker_t* p, const spell_data_t* s ) : Base( n, p, s )
    {
      base_t::dual = true;
      base_t::dot_duration = p->talent.ruby_embers.ok() ? base_t::dot_duration : 0_ms;
    }

    int n_targets() const override
    {
      if ( auto n = base_t::p()->buff.leaping_flames->check() )
        return 1 + n;
      else
        return Base::n_targets();
    }

    std::vector<player_t*>& target_list() const override
    {
      auto& tl = Base::target_list();

      if ( base_t::is_aoe() && as<int>( tl.size() ) > base_t::n_targets() )
      {
        // always hit the target, so if it exists make sure it's first
        auto start_it = tl.begin() + ( tl[ 0 ] == base_t::target ? 1 : 0 );

        // randomize remaining targets
        base_t::rng().shuffle( start_it, tl.end() );
      }

      return tl;
    }

    void impact( action_state_t* s ) override
    {
      Base::impact( s );

      if ( base_t::p()->talent.snapfire.ok() )
        base_t::p()->buff.snapfire->trigger();
    }
  };

  struct living_flame_damage_t : public living_flame_base_t<evoker_spell_t>
  {
    living_flame_damage_t( evoker_t* p ) : base_t( "living_flame_damage", p, p->spec.living_flame_damage ) {}

    double bonus_da( const action_state_t* s ) const override
    {
      auto da = base_t::bonus_da( s );

      da += p()->buff.scarlet_adaptation->check_value();

      return da;
    }

    double composite_da_multiplier( const action_state_t* s ) const override
    {
      auto da = base_t::composite_da_multiplier( s );

      da *= 1.0 + p()->buff.iridescence_red->check_value();

      return da;
    }
  };

  struct living_flame_heal_t : public living_flame_base_t<heals::evoker_heal_t>
  {
    living_flame_heal_t( evoker_t* p ) : base_t( "living_flame_heal", p, p->spec.living_flame_heal ) {}

    void execute() override
    {
      base_t::execute();

      if ( p()->talent.ancient_flame.ok() )
        p()->buff.ancient_flame->trigger();
    }
  };

  action_t* damage;
  action_t* heal;
  bool cast_heal;

  living_flame_t( evoker_t* p, std::string_view options_str )
    : evoker_spell_t( "living_flame", p, p->find_class_spell( "Living Flame" ) ), cast_heal( false )
  {

    damage        = p->get_secondary_action<living_flame_damage_t>( "living_flame_damage" );
    damage->stats = stats;

    // TODO: implement option to cast heal instead
    heal = p->get_secondary_action<living_flame_heal_t>( "living_flame_heal" );

    add_option( opt_bool( "heal", cast_heal ) );
    parse_options( options_str );
  }

  bool has_amount_result() const override
  {
    return damage->has_amount_result();
  }

  void execute() override
  {
    evoker_spell_t::execute();

    damage->execute_on_target( target );

    p()->buff.ancient_flame->expire();
    p()->buff.leaping_flames->expire();
    p()->buff.scarlet_adaptation->expire();

    if ( p()->talent.ruby_essence_burst.ok() &&
         ( p()->buff.dragonrage->up() || rng().roll( p()->talent.ruby_essence_burst->effectN( 1 ).percent() ) ) )
    {
      p()->buff.essence_burst->trigger();
      p()->proc.ruby_essence_burst->occur();
    }
  }
};

struct obsidian_scales_t : public evoker_spell_t
{
  obsidian_scales_t( evoker_t* p, std::string_view options_str )
    : evoker_spell_t( "obsidian_scales", p, p->talent.obsidian_scales, options_str )
  {}
};

struct quell_t : public evoker_spell_t
{
  quell_t( evoker_t* p, std::string_view options_str ) : evoker_spell_t( "quell", p, p->talent.quell, options_str )
  {
    may_miss = may_glance = may_block = may_dodge = may_parry = may_crit = harmful = false;
    ignore_false_positive = use_off_gcd = is_interrupt = true;
  }

  bool target_ready( player_t* t ) override
  {
    if ( !t->debuffs.casting->check() )
      return false;

    return evoker_spell_t::target_ready( t );
  }

  void execute() override
  {
    evoker_spell_t::execute();

    // always assume succes since action cannot be used unless target is casting
    if ( p()->talent.roar_of_exhilaration.ok() )
    {   
      p()->resource_gain(
          RESOURCE_ESSENCE,
          p()->talent.roar_of_exhilaration->effectN( 1 ).trigger()->effectN( 1 ).resource( RESOURCE_ESSENCE ),
          p()->gain.roar_of_exhilaration );
    }
  }
};

struct shattering_star_t : public evoker_spell_t
{
  shattering_star_t( evoker_t* p, std::string_view options_str )
    : evoker_spell_t( "shattering_star", p, p->talent.shattering_star, options_str )
  {
    aoe = as<int>( data().effectN( 1 ).base_value() * ( 1.0 + p->talent.eternitys_span->effectN( 2 ).percent() ) );

    if ( p->talent.arcane_vigor.ok() )
    {
      energize_type = action_energize::ON_CAST;
      energize_amount +=
          p->find_spelleffect( p->talent.arcane_vigor, A_ADD_FLAT_MODIFIER, P_EFFECT_4, &data() )->base_value();
    }
  }

  void impact( action_state_t* s ) override
  {
    evoker_spell_t::impact( s );

    if ( result_is_hit( s->result ) )
      td( s->target )->debuffs.shattering_star->trigger();
  }
};

struct tip_the_scales_t : public evoker_spell_t
{
  tip_the_scales_t( evoker_t* p, std::string_view options_str )
    : evoker_spell_t( "tip_the_scales", p, p->talent.tip_the_scales, options_str )
  {}

  void execute() override
  {
    evoker_spell_t::execute();

    p()->buff.tip_the_scales->trigger();
  }
};

struct pyre_t : public evoker_spell_t
{
  struct pyre_damage_t : public evoker_spell_t
  {
    pyre_damage_t( evoker_t* p, std::string_view name_str )
      : evoker_spell_t( name_str, p, p->talent.pyre->effectN( 1 ).trigger() )
    {
      dual = true;
      aoe  = -1;
    }
  };

  action_t* damage;

  pyre_t( evoker_t* p, std::string_view options_str ) : pyre_t( p, "pyre", p->talent.pyre, options_str ) {}

  pyre_t( evoker_t* p, std::string_view n, const spell_data_t* s, std::string_view o = {} )
    : evoker_spell_t( n, p, s, o )
  {
    damage = p->get_secondary_action<pyre_damage_t>( name_str + "_damage", name_str + "_damage" );
    damage->stats = stats;
    damage->proc = true;

    if ( p->action.volatility )
      add_child( p->action.volatility );

    // Charged blast is consumed on cast, but we need it to apply to the damage action that is executed on impact. We
    // snapshot ONLY the da mul from charged blast and iridescence and pass it along to the damage action state.
    snapshot_flags |= STATE_MUL_DA;
  }

  bool has_amount_result() const override
  {
    return damage->has_amount_result();
  }

  void execute() override
  {
    evoker_spell_t::execute();

    p()->buff.charged_blast->expire();
  }

  double composite_da_multiplier( const action_state_t* /*s*/ ) const override
  {
    // The base da_mul is NOT called here since we only want to capture the multiplier from charged blast & iridescence
    // in order to pass it on to the damage action state.
    double da = 1.0;
    da *= 1.0 + p()->buff.charged_blast->check_stack_value();
    da *= 1.0 + p()->buff.iridescence_red->check_value();

    return da;
  }

  void impact( action_state_t* s ) override
  {
    evoker_spell_t::impact( s );

    // The captured da mul from charged blast and iridescence is passed along to the damage action state.
    auto state = damage->get_state();
    state->target = s->target;
    damage->snapshot_state( state, damage->amount_type( state ) );
    state->da_multiplier *= s->da_multiplier;
    damage->schedule_execute( state );

    // TODO: How many times can volatility chain?
    if ( p()->action.volatility && rng().roll( p()->talent.volatility->effectN( 1 ).percent() ) )
    {
      const auto& tl = p()->action.volatility->target_list();
      p()->action.volatility->execute_on_target( tl[ rng().range( tl.size() ) ] );
    }
  }
};

struct dragonrage_t : public evoker_spell_t
{
  struct dragonrage_damage_t : public pyre_t
  {
    dragonrage_damage_t( evoker_t* p ) : pyre_t( p, "dragonrage_pyre", p->talent.dragonrage->effectN( 2 ).trigger() )
    {
      name_str_reporting = "pyre";
      s_data_reporting   = p->talent.pyre;

      // spell has 30yd range but the effect to launch pyre only has 25yd
      range = data().effectN( 1 ).radius();
      aoe = as<int>( data().effectN( 1 ).base_value() );
    }

    std::vector<player_t*>& target_list() const override
    {
      auto& tl = pyre_t::target_list();

      if ( as<int>( tl.size() ) > n_targets() )
        rng().shuffle( tl.begin(), tl.end() );

      return tl;
    }
  };

  action_t* damage;
  unsigned max_targets;

  dragonrage_t( evoker_t* p, std::string_view options_str )
    : evoker_spell_t( "dragonrage", p, p->talent.dragonrage, options_str ),
      max_targets( as<unsigned>( data().effectN( 2 ).trigger()->effectN( 1 ).base_value() ) )
  {
    damage = p->get_secondary_action<dragonrage_damage_t>( "dragonrage_pyre" );
    add_child( damage );
  }

  school_e get_school() const override
  {
    return damage->school;
  }

  void execute() override
  {
    evoker_spell_t::execute();

    p()->buff.dragonrage->trigger();
    damage->execute_on_target( target );
  }
};

}  // end namespace spells

// ==========================================================================
// Evoker Character Definitions
// ==========================================================================

evoker_td_t::evoker_td_t( player_t* target, evoker_t* evoker )
  : actor_target_data_t( target, evoker ),
    dots(),
    debuffs()
{
  dots.fire_breath = target->get_dot( "fire_breath_damage", evoker );
  dots.disintegrate = target->get_dot( "disintegrate", evoker );

  debuffs.shattering_star = make_buff( *this, "shattering_star_debuff", evoker->talent.shattering_star )
    ->set_cooldown( 0_ms )
    ->apply_affecting_aura( evoker->talent.focusing_iris );
}

evoker_t::evoker_t( sim_t* sim, std::string_view name, race_e r )
  : player_t( sim, EVOKER, name, r ),
    option(),
    action(),
    buff(),
    spec(),
    talent(),
    benefit(),
    cooldown(),
    gain(),
    proc(),
    rppm(),
    uptime()
{
  cooldown.eternity_surge = get_cooldown( "eternity_surge" );
  cooldown.fire_breath    = get_cooldown( "fire_breath" );
  cooldown.firestorm      = get_cooldown( "firestorm" );

  resource_regeneration = regen_type::DYNAMIC;
  regen_caches[ CACHE_HASTE ] = true;
  regen_caches[ CACHE_SPELL_HASTE ] = true;
}

void evoker_t::init_action_list()
{
  // 2022-08-07: Healing is not supported
  if ( !sim->allow_experimental_specializations && primary_role() == ROLE_HEAL )
  {
    if ( !quiet )
      sim->error( "Role heal for evoker '{}' is currently not supported.", name() );

    quiet = true;
    return;
  }

  if ( !action_list_str.empty() )
  {
    player_t::init_action_list();
    return;
  }
  clear_action_priority_lists();

  switch ( specialization() )
  {
    case EVOKER_DEVASTATION:
      evoker_apl::devastation( this );
      break;
    case EVOKER_PRESERVATION:
      evoker_apl::preservation( this );
      break;
    default:
      evoker_apl::no_spec( this );
      break;
  }

  use_default_action_list = true;

  player_t::init_action_list();
}

void evoker_t::init_gains()
{
  player_t::init_gains();

  gain.roar_of_exhilaration = get_gain( "Roar of Exhilaration" );
}

void evoker_t::init_procs()
{
  player_t::init_procs();

  proc.ruby_essence_burst  = get_proc( "Ruby Essence Burst" );
  proc.azure_essence_burst = get_proc( "Azure Essence Burst" );
  proc.eye_of_infinity           = get_proc( "eye_of_infinity" );
}

void evoker_t::init_base_stats()
{
  if ( base.distance < 1 )
    base.distance = 25;

  player_t::init_base_stats();

  base.spell_power_per_intellect = 1.0;

  resources.base[ RESOURCE_ESSENCE ] = 5 + find_spelleffect( talent.power_nexus, A_MOD_MAX_RESOURCE )->base_value();
  // TODO: confirm base essence regen. currently estimated at 1 per 5s base
  resources.base_regen_per_second[ RESOURCE_ESSENCE ] = 0.2 * ( 1.0 + talent.innate_magic->effectN( 1 ).percent() );
}

void evoker_t::init_spells()
{
  player_t::init_spells();

  // Evoker Talents
  auto CT = [ this ]( std::string_view n ) { return find_talent_spell( talent_tree::CLASS, n ); };
  auto ST = [ this ]( std::string_view n ) { return find_talent_spell( talent_tree::SPECIALIZATION, n ); };

  // Class Traits
  talent.landslide            = CT( "Landslide" );  // Row 1
  talent.obsidian_scales      = CT( "Obsidian Scales" );
  talent.expunge              = CT( "Expunge" );
  talent.natural_convergence  = CT( "Natural Convergence" );  // Row 2
  talent.forger_of_mountains  = CT( "Forger of Mountains" );  // Row 3
  talent.innate_magic         = CT( "Innate Magic" );
  talent.obsidian_bulwark     = CT( "Obsidian Bulwark" );
  talent.enkindled            = CT( "Enkindled" );
  talent.scarlet_adaptation   = CT( "Scarlet Adaptation" );
  talent.quell                = CT( "Quell" );  // Row 4
  talent.tailwind             = CT( "Tailwind" );
  talent.roar_of_exhilaration = CT( "Roar of Exhilaration" );  // Row 5
  talent.instinctive_arcana   = CT( "Instinctive Arcana" );
  talent.tip_the_scales       = CT( "Tip the Scales" );
  talent.attuned_to_the_dream = CT( "Attuned to the Dream" );  // healing received NYI
  talent.draconic_legacy      = CT( "Draconic Legacy" );  // Row 6
  talent.tempered_scales      = CT( "Tempered Scales" );
  talent.extended_flight      = CT( "Extended Flight" );
  talent.bountiful_bloom      = CT( "Bountiful Bloom" );
  talent.blast_furnace        = CT( "Blast Furnace" );  // Row 7
  talent.exuberance           = CT( "Exuberance" );
  talent.ancient_flame        = CT( "Ancient Flame" );
  talent.protracted_talons    = CT( "Protracted Talons" );  // Row 8
  talent.lush_growth          = CT( "Lush Growth" );
  talent.leaping_flames       = CT( "Leaping Flames" );  // Row 9
  talent.aerial_mastery       = CT( "Aerial Mastery" );
  // Devastation Traits
  talent.pyre                   = ST( "Pyre" );                // Row 1
  talent.ruby_essence_burst     = ST( "Ruby Essence Burst" );  // Row 2
  talent.azure_essence_burst    = ST( "Azure Essence Burst" );
  talent.dense_energy           = ST( "Dense Energy" );  // Row 3
  talent.imposing_presence      = ST( "Imposing Presence" );
  talent.eternity_surge         = ST( "Eternity Surge" );
  talent.volatility             = ST( "Volatility" );  // Row 4
  talent.power_nexus            = ST( "Power Nexus" );
  talent.dragonrage             = ST( "Dragonrage" );
  talent.lay_waste              = ST( "Lay Waste" );
  talent.arcane_intensity       = ST( "Arcane Intensity" );
  talent.ruby_embers            = ST( "Ruby Embers" );  // Row 5
  talent.engulfing_blaze        = ST( "Engulfing Blaze" );
  talent.animosity              = ST( "Animosity" );
  talent.essence_attunement     = ST( "Essence Attunement" );
  talent.firestorm              = ST( "Firestorm" );  // Row 6
  talent.heat_wave              = ST( "Heat Wave" );
  talent.titanic_wrath          = ST( "Titanic Wrath" );
  talent.honed_aggression       = ST( "Honed Aggression" );
  talent.eternitys_span         = ST( "Eternity's Span" );
  talent.eye_of_infinity        = ST( "Eye of Infinity" );
  talent.casuality              = ST( "Causality" );
  talent.catalyze               = ST( "Catalyze" );  // Row 7
  talent.tyranny                = ST( "Tyranny" );
  talent.charged_blast          = ST( "Charged Blast" );
  talent.shattering_star        = ST( "Shattering Star" );
  talent.snapfire               = ST( "Snapfire" );  // Row 8
  talent.font_of_magic          = ST( "Font of Magic" );
  talent.onyx_legacy            = ST( "Onyx Legacy" );
  talent.spellweavers_dominance = ST( "Spellweaver's Dominance" );
  talent.focusing_iris          = ST( "Focusing Iris" );
  talent.arcane_vigor           = ST( "Arcane Vigor" );
  talent.burnout                = ST( "Burnout" );  // Row 9
  talent.imminent_destruction   = ST( "Imminent Destruction" );
  talent.scintillation          = ST( "Scintillation" );
  talent.power_swell            = ST( "Power Swell" );
  talent.feed_the_flames        = ST( "Feed the Flames" );  // Row 10
  talent.everburning_flame      = ST( "Everburning Flame" );
  talent.cascading_power        = ST( "Cascading Power" );
  talent.iridescence            = ST( "Iridescence" );
  // Preservation Traits

  // Evoker Specialization Spells
  spec.evoker              = find_spell( 353167 );  // TODO: confirm this is the class aura
  spec.devastation         = find_specialization_spell( "Devastation Evoker" );
  spec.preservation        = find_specialization_spell( "Preservation Evoker" );
  spec.living_flame_damage = find_spell( 361500 );
  spec.living_flame_heal   = find_spell( 361509 );
}

void evoker_t::create_actions()
{
  using namespace spells;
  using namespace heals;

  if ( talent.volatility.ok() )
  {
    auto vol = get_secondary_action<pyre_t>( "pyre_volatility", "pyre_volatility", talent.pyre );
    vol->s_data_reporting = talent.volatility;
    vol->name_str_reporting = "volatility";
    action.volatility = vol;
  }

  player_t::create_actions();
}

void evoker_t::create_buffs()
{
  player_t::create_buffs();

  using namespace buffs;

  // Baseline Abilities
  buff.essence_burst =
      make_buff( this, "essence_burst", find_spell( specialization() == EVOKER_DEVASTATION ? 359618 : 369299 ) )
          ->apply_affecting_aura( talent.essence_attunement );

  buff.hover = make_buff( this, "hover", find_class_spell( "Hover" ) )
    ->set_cooldown( 0_ms )
    ->set_default_value_from_effect_type( A_MOD_INCREASE_SPEED );

  buff.tailwind = make_buff( this, "tailwind", find_spelleffect( talent.tailwind, A_PROC_TRIGGER_SPELL )->trigger() )
    ->set_default_value_from_effect( 1 );

  // Class Traits
  buff.ancient_flame = make_buff( this, "ancient_flame", find_spell( 375583 ) );

  buff.leaping_flames = make_buff( this, "leaping_flames", find_spell( 370901 ) );

  buff.obsidian_scales = make_buff( this, "obsidian_scales", talent.obsidian_scales )
    ->set_cooldown( 0_ms );

  buff.scarlet_adaptation = make_buff( this, "scarlet_adaptation", find_spell( 372470 ) );

  buff.tip_the_scales = make_buff( this, "tip_the_scales", talent.tip_the_scales )
    ->set_cooldown( 0_ms );

  // Devastation Traits
  buff.burnout = make_buff( this, "burnout", find_spell( 375802 ) )->set_cooldown( talent.burnout->cooldown() );

  buff.charged_blast = make_buff( this, "charged_blast", talent.charged_blast->effectN( 1 ).trigger() )
    ->set_default_value_from_effect( 1 );

  buff.dragonrage = make_buff( this, "dragonrage", talent.dragonrage );

  buff.iridescence_blue = make_buff( this, "iridescence_blue", find_spell( 386399 ) )
    ->set_default_value_from_effect( 1 );
  buff.iridescence_blue->set_initial_stack( buff.iridescence_blue->max_stack() );

  buff.iridescence_red = make_buff( this, "iridescence_red", find_spell( 386353 ) )
    ->set_default_value_from_effect( 1 );
  buff.iridescence_red->set_initial_stack( buff.iridescence_red->max_stack() );

  buff.power_swell = make_buff( this, "power_swell", find_spell( 376850 ) )
    ->set_affects_regen( true )
    ->set_default_value_from_effect_type( A_MOD_POWER_REGEN_PERCENT )
    ->set_duration( talent.power_swell->effectN( 1 ).time_value() );

  buff.snapfire = make_buff( this, "snapfire", talent.snapfire->effectN( 1 ).trigger() )
    ->set_chance( talent.snapfire->effectN( 1 ).percent() )
    ->set_default_value_from_effect( 2 )
    ->set_stack_change_callback( [ this ]( buff_t* b, int, int new_ ) {
      if ( new_ )
        cooldown.firestorm->adjust( b->data().effectN( 3 ).time_value() );
    } );

  // Preservation Traits
}

void evoker_t::create_options()
{
  player_t::create_options();
}

void evoker_t::analyze( sim_t& sim )
{
  // For proper DPET analysis, we need to treat empowered spell stat objs as non-channelled so the dot ticks from fire
  // breath do not get summed up into total execute time. All empowered spells have a release spell that is pushed onto
  // secondary_action_list, which we can use to find stat objs for empowered actions without having to go through the
  // entire action_list. See empower_charge_spell_t::tick() for more notes.
  for ( auto a : secondary_action_list )
  {
    if ( auto emp = dynamic_cast<spells::empowered_charge_spell_t*>( a->stats->action_list[ 0 ] ) )
      range::for_each( emp->stats->action_list, []( action_t* a ) { a->channeled = false; } );
  }

  player_t::analyze( sim );
}

void evoker_t::reset()
{
  player_t::reset();
}

void evoker_t::copy_from( player_t* source )
{
  player_t::copy_from( source );

  option = debug_cast<evoker_t*>( source )->option;
}

void evoker_t::merge( player_t& other )
{
  player_t::merge( other );
}

std::string evoker_t::default_potion() const
{
  return evoker_apl::potion( this );
}

std::string evoker_t::default_flask() const
{
  return evoker_apl::flask( this );
}

std::string evoker_t::default_food() const
{
  return evoker_apl::food( this );
}

std::string evoker_t::default_rune() const
{
  return evoker_apl::rune( this );
}

std::string evoker_t::default_temporary_enchant() const
{
  return evoker_apl::temporary_enchant( this );
}

const evoker_td_t* evoker_t::find_target_data( const player_t* target ) const
{
  return target_data[ target ];
}

evoker_td_t* evoker_t::get_target_data( player_t* target ) const
{
  evoker_td_t*& td = target_data[ target ];
  if ( !td )
    td = new evoker_td_t( target, const_cast<evoker_t*>( this ) );
  return td;
}

void evoker_t::apply_affecting_auras( action_t& action )
{
  player_t::apply_affecting_auras( action );

  // Baseline Auras
  action.apply_affecting_aura( spec.evoker );
  action.apply_affecting_aura( spec.devastation );
  action.apply_affecting_aura( spec.preservation );

  // Class Traits
  action.apply_affecting_aura( talent.aerial_mastery );
  action.apply_affecting_aura( talent.attuned_to_the_dream );
  action.apply_affecting_aura( talent.blast_furnace );
  action.apply_affecting_aura( talent.bountiful_bloom );
  action.apply_affecting_aura( talent.enkindled );
  action.apply_affecting_aura( talent.extended_flight );
  action.apply_affecting_aura( talent.forger_of_mountains );
  action.apply_affecting_aura( talent.lush_growth );
  action.apply_affecting_aura( talent.natural_convergence );
  action.apply_affecting_aura( talent.obsidian_bulwark );

  // Devastaion Traits
  action.apply_affecting_aura( talent.arcane_intensity );
  action.apply_affecting_aura( talent.dense_energy );
  action.apply_affecting_aura( talent.engulfing_blaze );
  action.apply_affecting_aura( talent.heat_wave );
  action.apply_affecting_aura( talent.honed_aggression );
  action.apply_affecting_aura( talent.imposing_presence );
  action.apply_affecting_aura( talent.lay_waste );
  action.apply_affecting_aura( talent.onyx_legacy );
  action.apply_affecting_aura( talent.spellweavers_dominance );

  // Preservation Traits
}

action_t* evoker_t::create_action( std::string_view name, std::string_view options_str )
{
  using namespace spells;

  if ( name == "azure_strike" )    return new    azure_strike_t( this, options_str );
  if ( name == "deep_breath" )     return new     deep_breath_t( this, options_str );
  if ( name == "disintegrate" )    return new    disintegrate_t( this, options_str );
  if ( name == "dragonrage" )      return new      dragonrage_t( this, options_str );
  if ( name == "eternity_surge" )  return new  eternity_surge_t( this, options_str );
  if ( name == "expunge" )         return new         expunge_t( this, options_str );
  if ( name == "fire_breath" )     return new     fire_breath_t( this, options_str );
  if ( name == "firestorm" )       return new       firestorm_t( this, options_str );
  if ( name == "hover" )           return new           hover_t( this, options_str );
  if ( name == "landslide" )       return new       landslide_t( this, options_str );
  if ( name == "living_flame" )    return new    living_flame_t( this, options_str );
  if ( name == "obsidian_scales" ) return new obsidian_scales_t( this, options_str );
  if ( name == "pyre" )            return new            pyre_t( this, options_str );
  if ( name == "quell" )           return new           quell_t( this, options_str );
  if ( name == "shattering_star" ) return new shattering_star_t( this, options_str );
  if ( name == "tip_the_scales" )  return new  tip_the_scales_t( this, options_str );

  return player_t::create_action( name, options_str );
}

std::unique_ptr<expr_t> evoker_t::create_expression( std::string_view expr_str )
{
  auto splits = util::string_split<std::string_view>( expr_str, "." );

  if ( util::str_compare_ci( expr_str, "essense" ) || util::str_compare_ci( expr_str, "essences" ) )
    return make_ref_expr( expr_str, resources.current[ RESOURCE_ESSENCE ] );

  return player_t::create_expression( expr_str );
}

// Stat & Multiplier overrides ==============================================

double evoker_t::composite_armor() const
{
  double a = player_t::composite_armor();

  if ( buff.obsidian_scales->check() )
    a *= 1.0 + buff.obsidian_scales->data().effectN( 1 ).percent();

  return a;
}

double evoker_t::composite_attribute_multiplier( attribute_e attr ) const
{
  double am = player_t::composite_attribute_multiplier( attr );

  if ( attr == ATTR_STAMINA )
    am *= 1.0 + talent.draconic_legacy->effectN( 1 ).percent();

  return am;
}

double evoker_t::composite_player_multiplier( school_e s ) const
{
  double m = player_t::composite_player_multiplier( s );

  if ( talent.instinctive_arcana.ok() && talent.instinctive_arcana->effectN( 1 ).has_common_school( s ) )
    m *= 1 + talent.instinctive_arcana->effectN( 1 ).percent();

  return m;
}

stat_e evoker_t::convert_hybrid_stat( stat_e stat ) const
{
  switch ( stat )
  {
    case STAT_STR_AGI_INT:
    case STAT_AGI_INT:
    case STAT_STR_INT:
      return STAT_INTELLECT;
    case STAT_STR_AGI:
    case STAT_SPIRIT:
    case STAT_BONUS_ARMOR:
      return STAT_NONE;
    default:
      return stat;
  }
}

double evoker_t::passive_movement_modifier() const
{
  double pmm = player_t::passive_movement_modifier();

  // hardcode 75% from spell desc; not found in spell data
  if ( talent.exuberance.ok() && health_percentage() > 75 )
    pmm += talent.exuberance->effectN( 1 ).percent();

  return pmm;
}

double evoker_t::resource_regen_per_second( resource_e resource ) const
{
  auto rrps = player_t::resource_regen_per_second( resource );

  if ( resource == RESOURCE_ESSENCE )
    rrps *= 1.0 + buff.power_swell->check_value();

  return rrps;
}

void evoker_t::target_mitigation( school_e school, result_amount_type rt, action_state_t* s )
{
  if ( buff.obsidian_scales->check() )
  {
    auto eff = buff.obsidian_scales->data().effectN( 2 );
    if ( eff.has_common_school( school ) )
      s->result_amount *= 1.0 + eff.percent();
  }

  if ( talent.tempered_scales.ok() )
  {
    auto eff = talent.tempered_scales->effectN( 1 );
    if ( eff.has_common_school( school ) )
      s->result_amount *= 1.0 + eff.percent();
  }

  player_t::target_mitigation( school, rt, s );
}

double evoker_t::temporary_movement_modifier() const
{
  auto tmm = player_t::temporary_movement_modifier();

  // TODO: confirm hover is a non-stacking temporary movement mod
  if ( buff.hover->check() )
    tmm = std::max( tmm, buff.hover->check_value() + buff.tailwind->check_value() );

  return tmm;
}

// Utility functions ========================================================

// Utility function to search spell data for matching effect.
// NOTE: This will return the FIRST effect that matches parameters.
const spelleffect_data_t* evoker_t::find_spelleffect( const spell_data_t* spell, effect_subtype_t subtype,
                                                      int misc_value, const spell_data_t* affected, effect_type_t type )
{
  for ( size_t i = 1; i <= spell->effect_count(); i++ )
  {
    const auto& eff = spell->effectN( i );

    if ( affected->ok() && !affected->affected_by_all( eff ) )
      continue;

    if ( eff.type() == type && eff.subtype() == subtype )
    {
      if ( misc_value != 0 )
      {
        if ( eff.misc_value1() == misc_value )
          return &eff;
      }
      else
        return &eff;
    }
  }

  return &spelleffect_data_t::nil();
}

// Return the appropriate spell when `base` is overriden to another spell by `passive`
const spell_data_t* evoker_t::find_spell_override( const spell_data_t* base, const spell_data_t* passive )
{
  if ( !passive->ok() )
    return base;

  auto id = as<unsigned>( find_spelleffect( passive, A_OVERRIDE_ACTION_SPELL, base->id() )->base_value() );
  if ( !id )
    return base;

  return find_spell( id );
}

/* Report Extension Class
 * Here you can define class specific report extensions/overrides
 */
class evoker_report_t : public player_report_extension_t
{
public:
  evoker_report_t( evoker_t& player ) : p( player ) {}

  void html_customsection( report::sc_html_stream& /*os*/ ) override {}

private:
  evoker_t& p;
};

// EVOKER MODULE INTERFACE ==================================================
struct evoker_module_t : public module_t
{
  evoker_module_t() : module_t( EVOKER ) {}

  player_t* create_player( sim_t* sim, std::string_view name, race_e r = RACE_NONE ) const override
  {
    auto p = new evoker_t( sim, name, r );
    p->report_extension = std::make_unique<evoker_report_t>( *p );
    return p;
  }
  bool valid() const override { return true; }
  void init( player_t* ) const override {}
  void static_init() const override {}
  void register_hotfixes() const override {}
  void combat_begin( sim_t* ) const override {}
  void combat_end( sim_t* ) const override {}
};
}

const module_t* module_t::evoker()
{
  static evoker_module_t m;
  return &m;
}
