
#ifndef LORAINE_TARGET_SYSTEM_HPP
#define LORAINE_TARGET_SYSTEM_HPP

#include "system.hpp"

class GameState;

class TargetSystem : public System {
  public:
   template <typename EffectT >
   void reset_targets(EffectT& effect);

   /**
    * @brief Returns the vector of manual targeters of any effects pertaining to the event ids.
    * @tparam ids the event ids in question
    * @param state the game state
    * @param card the card in question
    * @return vector of targeter pointers
    */
   template < events::EventID... ids >
   std::vector< Targeter* > manual_targeters(GameState& state, entt::entity card);

  private:
   template < events::EventID first_id, events::EventID... rest_ids >
   static void _manual_targeters_impl(
      entt::registry& registry,
      entt::entity card,
      std::vector< Targeter* >& targeter_vec);

};


#include "loraine/core/gamestate.h"


template < events::EventID... ids >
std::vector< Targeter* > TargetSystem::manual_targeters(
   GameState& state,
   entt::entity card)
{
   std::vector< Targeter* > targeter_vec;
   if(m_registry->any_of< EffectVector< ids >... >(card)) {
      _manual_targeters_impl< ids... >(state, card, targeter_vec);
   }
   return targeter_vec;
}


template < events::EventID first_id, events::EventID... rest_ids >
void TargetSystem::_manual_targeters_impl(
   entt::registry& registry,
   entt::entity card,
   std::vector< Targeter* >& targeter_vec)
{
   if(registry.all_of< EffectVector< first_id > >(card)) {
      for(const auto& effect : registry.get< EffectVector< first_id > >(card)) {
         if(utils::has_value(effect.first.targeter)
            && effect.first.targeter.value().mode == Targeter::Mode::MANUAL) {
            targeter_vec.emplace_back(&effect.first.targeter.value());
         }
      }
   }
   _manual_targeters_impl< rest_ids... >(registry, card, targeter_vec);
}

template < typename EffectT >
void TargetSystem::reset_targets(EffectT& effect)
{
   effect.ability.targets.reset();
   effect.condition.targets.reset();
}

#endif  // LORAINE_TARGET_SYSTEM_HPP
