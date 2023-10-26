#pragma once

namespace monk
{

// Forward Declare each ability that may be externally referenced from sc_monk_abilities.cpp
// Make sure this file remains clang-format'ed.

// Generic meta-action types
template <class Base>
struct monk_action_t;
struct monk_attack_t;
struct monk_heal_t;
struct monk_absorb_t;

// Concrete action types
struct blackout_kick_t;
struct auto_attack_t;
struct chi_surge_t;
struct weapons_of_order_t;
}  // namespace monk
