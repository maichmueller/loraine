#ifndef LORAINE_SORT_H
#define LORAINE_SORT_H

#include "loraine/effects/effect.h"
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
std::vector< sptr< IGameEffect< EventType > > > next(
   entt::registry& registry,
   std::vector< entt::entity > scheduled_entities,
   const EventType& event)
{
   auto view = registry.view<
      tag::subscriber< EventType::id >,  // all entities that hold the subscriber tag
      Position,  // we need their Position on the board component
      sptr< IGameEffect< EventType > > >();  // we need their specific effects

   // store the effects that are to be returned in an interrim vector to sort them by position
   using effect_pos_pair = std::tuple< Position, sptr< IGameEffect< EventType > >, entt::entity >;
   auto pair_comp = [](const auto& pair1, const auto& pair2) {
      auto& pos1 = pair1.second;
      auto& pos2 = pair2.second;

      // TODO: Let this become a debug mode check
      // if the entity is in any of these zones, then their subscription is still valid and no
      // logic error is present. The Graveyard is included because of last breath effects
      // bringing back the dead (e.g. Shark Charriot)
      constexpr auto valid_zones = std::array< Zone, 4 >{
         Zone::BOARD, Zone::CAMP, Zone::HAND, Zone::GRAVEYARD};
      if(algo::any_of({pos1.zone(), pos2.zone()}, [&](const Zone& zone) {
            return algo::contains(valid_zones, zone);
         })) {
         throw std::logic_error(
            "A still subscribed entitiy is not in any of the valid Game Zones: \n\t"
            "[BOARD, CAMP, HAND, GRAVEYARD]");
      }

      // if their position is further to the left (i.e. lower value),
      // then their execution should generally happen first
      return pos1.index() < pos2.index();
   };
   std::priority_queue< effect_pos_pair, std::vector< effect_pos_pair >, decltype(pair_comp) >
      effect_queue;
   for(auto entity : view) {
      if(algo::contains(scheduled_entities, entity)) {
         effect_queue.push(std::make_tuple(
            view.template get< Position >(entity),
            view.template get< sptr< IGameEffect< EventType > > >(entity),
            entity));
      }
   }
   std::pair< entt::entity, sptr< IGameEffect< EventType > > > next = std::get< 1, 2 >(
      effect_queue.top());

   return next;
}
}  // namespace schedule
#endif  // LORAINE_SORT_H
