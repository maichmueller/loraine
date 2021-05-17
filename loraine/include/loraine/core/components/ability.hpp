
#ifndef LORAINE_ABILITY_HPP
#define LORAINE_ABILITY_HPP

#include <functional>

#include "loraine/core/components/targeter.hpp"
#include "loraine/effects/effectdefs.hpp"
#include "loraine/events/event_id.h"
#include "loraine/events/helpers.h"

class GameState;

template < events::EventID event_id >
struct Ability {
   using EventType = helpers::id_to_event_t< event_id >;
   using FuncType = std::function< void(GameState&, const EventType&) >;

   inline void operator()(GameState& state, const EventType& event) { impl(state, event); }

   /// the actual implementation of the ability
   FuncType impl;
   /// the point in time in which the ability should be connected
   RegistrationTime registration_time;
   /// the style of the ability, i.e. targeting, aura, etc.
   Style style;
   /// the targeter of the ability
   std::optional< Targeter > targeter;
   /// whether the ability has been registered already
   bool is_registered = false;
   /// whether the effect has been used up and should be disconnected (e.g. one time effects)
   bool is_consumed = false;
};

template < events::EventID event_id >
struct AbilityCondition {
   using FuncType = typename Ability< event_id >::FuncType;

   inline void operator()(GameState& state) { impl(state); }

   /// the actual implementation of the condition
   FuncType impl;
};

template < events::EventID event_id >
using Effect = std::pair< Ability< event_id >, AbilityCondition< event_id > >;

template < events::EventID event_id >
using EffectVector = std::vector< Effect< event_id > >;

#endif  // LORAINE_ABILITY_HPP
