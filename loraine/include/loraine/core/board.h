
#ifndef LORAINE_BOARD_H
#define LORAINE_BOARD_H

#include <entt/entt.hpp>
#include <functional>
#include <iostream>
#include <queue>
#include <utility>
#include <variant>

#include "gamedefs.h"
#include "loraine/utils/types.h"

class Board {
  public:
   using Container = std::vector< entt::entity >;
   using Queue = std::queue< entt::entity >;

   Board(size_t camp_size, size_t bf_size)
       : m_size_max_camp(camp_size), m_size_max_bf(bf_size), m_container(), m_q_camp(), m_q_bf()
   {
      _reserve_space();
   }

   Board(
      size_t camp_size,
      size_t bf_size,
      Container container,
      SymArr< Queue > camp_queues,
      SymArr< Queue > bf_queues)
       : m_size_max_camp(camp_size),
         m_size_max_bf(bf_size),
         m_container(std::move(container)),
         m_q_camp(std::move(camp_queues)),
         m_q_bf(std::move(bf_queues))
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
   template < typename Predicate >
   [[nodiscard]] size_t count_if(Team team, bool in_camp, Predicate filter) const;

   [[nodiscard]] auto max_size_bf() const { return m_size_max_bf; }
   [[nodiscard]] auto max_size_camp() const { return m_size_max_camp; }

   [[nodiscard]] std::vector< entt::entity > camp_units(entt::registry& registry, Team team) const;

   void add_to_bf(entt::registry& registry, entt::entity card, size_t idx);
   void add_to_bf(entt::registry& registry, entt::entity card);
   void add_to_camp(entt::registry& registry, entt::entity card);

   void add_to_camp_queue(entt::registry& registry, entt::entity card);
   void add_to_camp_queue(entt::registry& registry, std::vector< entt::entity >&& units);
   void add_to_bf_queue(entt::registry& registry, entt::entity unit);
   void add_to_bf_queue(entt::registry& registry, std::vector< entt::entity >&& units);

  private:
   /// the max number of cards in the camp at any time
   size_t m_size_max_camp;
   /// the max number of units on the battlefield at any time
   size_t m_size_max_bf;
   // the ContainerType holding all entities
   Container m_container;
   /// the queue for adding new cards to the camp
   SymArr< Queue > m_q_camp;
   // the queue for newly summoned units on the battlefield
   SymArr< Queue > m_q_bf;

   /// internal logic methods

   void _process_camp_queues(entt::registry& registry);

   inline bool _check_boundaries()
   {
      return m_container.size() < Team::n_team * (m_size_max_camp + m_size_max_bf);
   }

   inline void _reserve_space()
   {
      m_container.reserve(Team::n_team * (m_size_max_camp + m_size_max_bf));
   }

   /**
    * Fills the chosen battlefield up with nullopt, so that at index `until` an actual
    * unit can be emplaced. This is required for e.g. deciding blockers at not all attacked
    * positions.
    * @param team Team,
    *   the team, whose battelfield should be filled
    * @param until size_t,
    *   the index until which the placeholders need to be put
    */

   inline void _fill_with_null(Team team, size_t until)
   {
      // The vector's logic goes:
      //    [  A  ][  B  ][    ][    ][    ][    ][    ]
      //       0      1   size()                   until
      // So everything from size() to index 'until' must be filled with empty placeholders.
      auto& bf = m_bf[team];
      for(int i = bf.size(); i < until; ++i) {
         bf.emplace_back(std::nullopt);
      }
   }
};

template < typename Predicate >
size_t Board::count_if(Team team, bool in_camp, Predicate filter) const
{
   size_t sum = 0;
   if(in_camp) {
      for(const auto& elem : m_camp.at(team)) {
         if(filter(elem)) {
            sum += 1;
         } else {
            break;
         }
      }
   } else {
      for(const auto& pot_unit : m_bf.at(team)) {
         if(utils::has_value(pot_unit) && filter(pot_unit.value())) {
            sum += 1;
         } else {
            break;
         }
      }
   }
   return sum;
}

#endif  // LORAINE_BOARD_H
