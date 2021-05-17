
#ifndef LORAINE_EFFECT_SYSTEM_HPP
#define LORAINE_EFFECT_SYSTEM_HPP

#include <entt/entt.hpp>

#include "loraine/core/components.hpp"
#include "loraine/core/gamedefs.h"
#include "loraine/core/schedule.h"
#include "loraine/effects/effectdefs.hpp"
#include "loraine/events/event_id.h"
#include "loraine/events/helpers.h"
#include "loraine/utils/utils.h"
#include "system.hpp"

class GameState;

/**
 * The base class for effects in the game.
 */
class EffectSystem: public ILogicSystem {
  public:
   EffectSystem() = default;

   template < events::EventID event_id, typename... Args >
   void trigger(Args&&... args)
   {
      // create the event corresponding to the ID first
      auto event = helpers::id_to_event_t< event_id >{std::forward< Args >(args)...};
      // get the subscribers to this event type
      auto view = m_registry->view< tag::subscriber< event_id > >();
      auto scheduled_entities = std::vector< entt::entity >{view.begin(), view.end()};
      // iterate over the scheduled entities, always popping the next one according to the order
      // policy of this event
      while(not scheduled_entities.empty()) {
         auto entity_to_trigger = schedule::next(*m_registry, scheduled_entities, event);
         m_registry->get<Ability<event_id>>(entity_to_trigger).impl(event);
         scheduled_entities.erase(algo::find(scheduled_entities, entity_to_trigger));
      }
   }

   template < events::EventID event_id >
   void give_effect(entt::entity entity, const Effect< event_id >& effect)
   {
      if(m_registry->all_of< EffectVector< event_id > >(entity)) {
         m_registry->patch< EffectVector< event_id > >(
            entity, [&effect](auto& eff_vec) { eff_vec.emplace_back(effect); });
      }
   }

   template < events::EventID event_id >
   inline void give_effect(
      entt::entity entity,
      const Ability< event_id >& ability,
      const AbilityCondition< event_id >& condition = AbilityCondition< event_id >([](const auto&) {
         return true;
      }))
   {
      give_effect(entity, Effect< event_id >(ability, condition));
   }
   template < events::EventID event_id >
   [[nodiscard]] bool is_targeting(const Effect< event_id >& effect) const
   {
      return effect.first.style == Style::TARGETING;
   }
   template < events::EventID event_id >
   [[nodiscard]] bool has_unconsumed_effects(entt::entity entity) const
   {
      if(m_registry->all_of< EffectVector< event_id > >(entity)) {
         return algo::any_of(
            m_registry->get< EffectVector< event_id > >(entity),
            [entity](const auto& effect) { return effect.first.is_consumed; });
      }
   }
   template < events::EventID event_id >
   [[nodiscard]] inline bool is_consumed(const Effect< event_id >& effect) const
   {
      return effect.first.is_consumed;
   }
   template < events::EventID event_id >
   [[nodiscard]] inline bool consume(const Effect< event_id >& effect) const
   {
      return effect.first.is_consumed = true;
   }
};

#endif  // LORAINE_EFFECT_SYSTEM_HPP
