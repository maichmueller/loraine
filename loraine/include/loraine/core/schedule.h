#ifndef LORAINE_SORT_H
#define LORAINE_SORT_H

#include <queue>

#include "loraine/core/components.h"
#include "loraine/utils/utils.h"

namespace schedule {
/**
 * @brief returns the next scheduled entity according to the event order policy.
 *
 * @tparam EventType
 * @param registry
 * @param scheduled_entities
 * @param event
 * @return
 */
template < class EventType >
entt::entity next(
   entt::registry& registry,
   std::vector< entt::entity > scheduled_entities,
   const EventType& event)
{
   auto view = registry.view<
      tag::subscriber< EventType::id >,  // all entities that hold the subscriber tag
      Position,  // we need their Position on the board component
      EffectVector< EventType::id > >();  // we need their specific effects

   // store the effects that are to be returned in an interrim vector to sort them by position
   // TODO: Adding Zone::CAMP here is only a temporary workaround! This needs to be identified
   //  better, as in what scheduling rules there may be
   using effect_pos_pair = std::
      tuple< Position< Zone::CAMP >, EffectVector< EventType::id >, entt::entity >;
   auto pair_comp = [](const auto& pair1, const auto& pair2) {
      auto& pos1 = pair1.second;
      auto& pos2 = pair2.second;

      // TODO: Let this become a debug mode check
      // if the entity is in any of these zones, then their subscription is still valid and no
      // logic error is present. The Graveyard is included because of last breath effects
      // bringing back the dead (e.g. Shark Charriot)
      constexpr auto valid_zones = std::array{
         Zone::CAMP, Zone::BATTLEFIELD, Zone::HAND, Zone::GRAVEYARD};
      if(algo::any_of({pos1.zone(), pos2.zone()}, [&](const Zone& zone) {
            return algo::contains(valid_zones, zone);
         })) {
         throw std::logic_error(
            "A still subscribed entity is not in any of the valid Game Zones: \n\t"
            "[CAMP, BATTLEFIELD, HAND, GRAVEYARD]");
      }

      // if their position is further to the left (i.e. lower value),
      // then their execution should generally happen first
      // TODO: verify if this statement is even correct. The order of some event effects may be
      //  simply random?
      return pos1.index() < pos2.index();
   };
   std::priority_queue< effect_pos_pair, std::vector< effect_pos_pair >, decltype(pair_comp) >
      effect_queue;
   for(auto entity : view) {
      if(algo::contains(scheduled_entities, entity)) {
         effect_queue.push(std::make_tuple(
            view.template get< Position >(entity),
            view.template get< EffectVector< EventType::id > >(entity),
            entity));
      }
   }

   return std::get< 0 >(effect_queue.top());
}
}  // namespace schedule
#endif  // LORAINE_SORT_H
