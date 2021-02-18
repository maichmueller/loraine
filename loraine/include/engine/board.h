
#ifndef LORAINE_BOARD_H
#define LORAINE_BOARD_H

#include <queue>
#include <utility>
#include <variant>

#include "rulesets.h"
#include "utils/types.h"

class Card;
class Unit;
class Landmark;

class Board {
   using BfType = std::vector< sptr< Card > >;
   using CampType = std::vector< sptr< Card > >;
   using CampQueueType = std::queue< sptr< Card > >;

   // the container holding battling units
   SymArr< BfType > m_battlefield;

   // the container holding summoned/played, alive units
   SymArr< CampType > m_camp;
   // the queue for newly summoned units
   SymArr< CampQueueType > m_camp_queue;

   inline bool _check_boundaries(Player player, bool in_camp)
   {
      return in_camp ? m_camp[player].size() < CAMP_SIZE
                     : m_battlefield[player].size() < BATTLEFIELD_SIZE;
   }
   inline void _reserve_space()
   {
      m_camp[RED].reserve(CAMP_SIZE);
      m_camp[BLUE].reserve(CAMP_SIZE);
   }

  public:
   Board() : m_battlefield(), m_camp(), m_camp_queue() { _reserve_space(); }
   Board(SymArr< BfType > bfs, SymArr< CampType > camps, SymArr< CampQueueType > queues)
       : m_battlefield(std::move(bfs)), m_camp(std::move(camps)), m_camp_queue(std::move(queues))
   {
      _reserve_space();
   }

   /*
    * Counts the units in the camp or the battlefield, subject to a filter on
    * the unit ptr, if needed.
    */
   [[nodiscard]] size_t count_units(
      Player player, bool in_camp, const std::function< bool(const sptr< Card >&) >& filter) const;

   [[nodiscard]] size_t count_units(Player player, bool in_camp) const;

   auto& get_battlefield(Player player) { return m_battlefield[player]; }
   [[nodiscard]] auto& get_battlefield(Player player) const { return m_battlefield[player]; }
   auto& get_camp(Player player) { return m_camp[player]; }
   [[nodiscard]] auto& get_camp(Player player) const { return m_camp[player]; }
   [[nodiscard]] std::vector< sptr< Unit > > get_camp_units(Player player) const;
   auto& get_camp_queue(Player player) { return m_camp_queue.at(player); }
   [[nodiscard]] auto& get_camp_queue(Player player) const { return m_camp_queue.at(player); }

   void add_to_queue(const sptr< Card >& card);
   void add_to_queue(std::vector< sptr< Card > >&& units);
};

#endif  // LORAINE_BOARD_H
