
#ifndef LORAINE_POSITION_SYSTEM_HPP
#define LORAINE_POSITION_SYSTEM_HPP

#include <loraine/core/config.h>

#include "loraine/core/board.h"
#include "loraine/core/components.h"
#include "loraine/core/gamedefs.h"
#include "system.h"

class PositionSystem: public ILogicSystem {
   /**
    * @brief Moves the card to the new zone at the given index.
    *
    * This method will update the position indices of the previously allocated zone of the card.
    * The Position component of each card should stay up-to-date this way for every move.
    */
  public:
   template < Zone new_zone >
   void move_to(entt::entity card, size_t index, const Config& cfg)
   {
      auto [pos, team] = m_registry.get< Position, Team >(card);
      _remove_loc_tags(card);
      pos.index = index;
      m_registry.emplace< tag::location< new_zone > >(card);
   }

  private:
   /**
    * @brief Reindexes the entities in a location after a removal.
    *
    * @tparam zone
    * @return
    */
   template < Zone zone >
   size_t _update_positions()
   {
      auto zone_view = m_registry.view< tag::location< zone >, Position >();
      std::vector< Position* > positions;
      zone_view.each([&](auto& entity, auto& z, auto& pos) { positions.emplace_back(pos); });
      std::sort(positions.begin(), positions.end(), [](const Position* p1, const Position* p2) {
        return p1->index < p2->index;
      });
      auto c = 0UL;
      for(auto* pos : positions) {
         pos->index = c++;
      }
      return c;
   }

   /**
    * @brief Removes all location tags of the card.
    *
    * Iterates through all possible zones, in which the card could be in, to strip its current one.
    * This method assumes, that each card has only been assigned a single location tag.
    *
    * @param card
    * @return
    */
   size_t _remove_loc_tags(entt::entity card) {
      _remove_loc_tags_impl< Zone(n_zone - 1) >(card);
   }

   template < Zone zone >
   size_t _remove_loc_tags_impl(entt::entity card)
   {
      if(auto view = m_registry.view< tag::location< zone > >(); view.contains(card)) {
         m_registry.remove< tag::location< zone > >(card);
         return _update_positions< zone >();
      }
      // if it wasn't this position, then we go on, to the remainder
      if constexpr(static_cast< size_t >(zone) > 0) {
         _remove_loc_tags_impl< static_cast< Zone >(static_cast< size_t >(zone) - 1) >(card);
      }
   }
};

#endif  // LORAINE_POSITION_SYSTEM_HPP
