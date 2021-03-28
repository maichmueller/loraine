
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
   using BfType = std::vector< sptr< Unit > >;
   using CampType = std::vector< sptr< FieldCard > >;
   using BfQueueType = std::queue< sptr< Unit > >;
   using CampQueueType = std::queue< sptr< FieldCard > >;

   Board(size_t camp_size, size_t bf_size)
       : m_camp_size_max(camp_size), m_bf_size_max(bf_size), m_bf(), m_camp(), m_camp_queue()
   {
      _reserve_space();
   }
   Board(size_t camp_size, size_t bf_size, SymArr< BfType > bfs, SymArr< CampType > camps)
       : m_camp_size_max(camp_size),
         m_bf_size_max(bf_size),
         m_bf(std::move(bfs)),
         m_camp(std::move(camps)),
         m_camp_queue()
   {
      _reserve_space();
   }
   Board(
      size_t camp_size,
      size_t bf_size,
      SymArr< BfType > bfs,
      SymArr< CampType > camps,
      SymArr< CampQueueType > camp_queues,
      SymArr< BfQueueType > bf_queues)
       : m_camp_size_max(camp_size),
         m_bf_size_max(bf_size),
         m_bf(std::move(bfs)),
         m_camp(std::move(camps)),
         m_camp_queue(std::move(camp_queues)),
         m_bf_queue(std::move(bf_queues))
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

   auto* battlefield(Team team) { return &m_bf[team]; }
   [[nodiscard]] auto* battlefield(Team team) const { return &m_bf[team]; }
   auto* camp(Team team) { return &m_camp[team]; }
   [[nodiscard]] auto* camp(Team team) const { return &m_camp[team]; }
   [[nodiscard]] std::vector< sptr< Unit > > camp_units(Team team) const;
   auto& camp_queue(Team team) { return m_camp_queue.at(team); }
   [[nodiscard]] auto& camp_queue(Team team) const { return m_camp_queue.at(team); }

   void move_to_bf(const sptr<Unit>& card, size_t idx);
   void move_to_bf(const sptr<Unit>& card);
   void move_to_camp(const sptr<Unit>& card);

   void add_to_camp_queue(const sptr< FieldCard >& card);
   void add_to_camp_queue(std::vector< sptr< FieldCard > >&& units);
   void add_to_bf_queue(const sptr< Unit >& unit);
   void add_to_bf_queue(std::vector< sptr< Unit > >&& units);

  private:
   size_t m_camp_size_max;
   size_t m_bf_size_max;
   // the ContainerType holding battling units
   SymArr< BfType > m_bf;
   // the ContainerType holding summoned/played, alive units
   SymArr< CampType > m_camp;
   // the m_queue for newly summoned units
   SymArr< CampQueueType > m_camp_queue;
   // the m_queue for newly summoned units on the battlefield
   SymArr< BfQueueType > m_bf_queue;

   inline bool _check_boundaries(Team team, bool in_camp)
   {
      return in_camp ? m_camp[team].size() < m_camp_size_max : m_bf[team].size() < m_bf_size_max;
   }
   inline void _reserve_space()
   {
      m_camp[RED].reserve(m_camp_size_max);
      m_camp[BLUE].reserve(m_camp_size_max);
      m_bf[RED].reserve(m_bf_size_max);
      m_bf[BLUE].reserve(m_bf_size_max);
   }

   /**
    * Fills the chosen battlefield up with nullptr placeholders, so that at index `until` an actual
    * unit can be emplaced. This is required for e.g. deciding blockers at not all attacked
    * positions.
    * @param team Team,
    *   the team, whose battelfield should be filled
    * @param until size_t,
    *   the index until which the placeholders need to be put
    */
   inline void _fill_with_nullptr(Team team, size_t until)
   {
      auto& bf = m_bf[team];
      for(auto i = 0; i < until - m_bf.size(); ++i) {
         bf.emplace_back(nullptr);
      }
   }
};

#endif  // LORAINE_BOARD_H
