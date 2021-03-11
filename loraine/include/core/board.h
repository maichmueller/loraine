
#ifndef LORAINE_BOARD_H
#define LORAINE_BOARD_H

#include <queue>
#include <utility>
#include <variant>

#include "gamedefs.h"
#include "utils/types.h"

class FieldCard;
class Unit;
class Landmark;

class Board {
  public:
   using BfType = std::vector< sptr< FieldCard > >;
   using CampType = std::vector< sptr< FieldCard > >;
   using CampQueueType = std::queue< sptr< FieldCard > >;

   Board(size_t camp_size, size_t bf_size)
       : m_camp_size(camp_size),
         m_battlefield_size(bf_size),
         m_battlefield(),
         m_camp(),
         m_camp_queue()
   {
      _reserve_space();
   }
   Board(
      size_t camp_size,
      size_t bf_size,
      SymArr< BfType > bfs,
      SymArr< CampType > camps,
      SymArr< CampQueueType > queues)
       : m_camp_size(camp_size),
         m_battlefield_size(bf_size),
         m_battlefield(std::move(bfs)),
         m_camp(std::move(camps)),
         m_camp_queue(std::move(queues))
   {
      _reserve_space();
   }
   /**
    * Counts the units in the camp or the battlefield, subject to a filter on
    * the common ptr, if needed.
    * @param team Team,
    *   the team whose units should be counted
    * @param in_camp bool,
    *   whether to count the units in the cmap or on the battlefield
    * @param filter Callable,
    *   the filter to use on the units. Can be used to search for units with specific attributes
    * @return size_t,
    *   the count
    */
   [[nodiscard]] size_t count_units(
      Team team,
      bool in_camp,
      const std::function< bool(const sptr< FieldCard >&) >& filter) const;
   [[nodiscard]] size_t count_units(Team team, bool in_camp) const;

   auto& battlefield(Team team) { return m_battlefield[team]; }
   [[nodiscard]] auto& battlefield(Team team) const { return m_battlefield[team]; }
   auto& camp(Team team) { return m_camp[team]; }
   [[nodiscard]] auto& camp(Team team) const { return m_camp[team]; }
   [[nodiscard]] std::vector< sptr< Unit > > camp_units(Team team) const;
   auto& camp_queue(Team team) { return m_camp_queue.at(team); }
   [[nodiscard]] auto& camp_queue(Team team) const { return m_camp_queue.at(team); }

   void add_to_queue(const sptr< FieldCard >& card);
   void add_to_queue(std::vector< sptr< FieldCard > >&& units);

  private:
   size_t m_camp_size;
   size_t m_battlefield_size;
   // the ContainerType holding battling units
   SymArr< BfType > m_battlefield;
   // the ContainerType holding summoned/played, alive units
   SymArr< CampType > m_camp;
   // the m_queue for newly summoned units
   SymArr< CampQueueType > m_camp_queue;

   inline bool _check_boundaries(Team team, bool in_camp)
   {
      return in_camp ? m_camp[team].size() < m_camp_size
                     : m_battlefield[team].size() < m_battlefield_size;
   }
   inline void _reserve_space()
   {
      m_camp[RED].reserve(m_camp_size);
      m_camp[BLUE].reserve(m_camp_size);
   }
};

#endif  // LORAINE_BOARD_H
