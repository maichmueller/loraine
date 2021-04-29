
#ifndef LORAINE_BOARD_SYSTEM_H
#define LORAINE_BOARD_SYSTEM_H

#include <entt/entt.hpp>
#include <functional>
#include <iostream>
#include <queue>
#include <utility>
#include <variant>

#include "loraine/core/components.h"
#include "loraine/core/gamedefs.h"
#include "loraine/core/systems/system.hpp"
#include "loraine/utils/utils.h"

class BoardSystem: public ILogicSystem {
  public:
   using Queue = std::queue< entt::entity >;

   BoardSystem(entt::registry& registry, size_t camp_size, size_t bf_size)
       : ILogicSystem(registry), m_size_max_camp(camp_size), m_size_max_bf(bf_size)
   {
   }

   BoardSystem(
      entt::registry& registry,
      size_t camp_size,
      size_t bf_size,
      SymArr< Queue > camp_queues,
      SymArr< Queue > bf_queues)
       : ILogicSystem(registry),
         m_size_max_camp(camp_size),
         m_size_max_bf(bf_size),
         m_q_camp(std::move(camp_queues)),
         m_q_bf(std::move(bf_queues))
   {
   }

   template < Zone zone >
   inline void on_move()
   {
      m_size_cache[static_cast< size_t >(zone)] += 1;
   }

   /**
    * @brief Moves the card to the new zone at the given index.
    *
    * This method will update the position indices of the previously allocated zone of the card.
    * The Position component of each card should stay up-to-date this way for every move.
    */
  public:
   template < Zone new_zone >
   bool move_to(entt::entity card, size_t index)
   {
      auto pos = m_registry.get< Position >(card);
      constexpr auto same_pos_check = [&, this]() {
         return m_registry.all_of< tag::location< new_zone > >(card) && pos.index == index
                || m_size_cache[static_cast< size_t >(new_zone)] == [&]() {
                      if constexpr(new_zone == Zone::BATTLEFIELD) {
                         return m_size_max_bf;
                      }
                      return m_size_max_camp;
                   }();
      };

      if constexpr(algo::contains({Zone::BATTLEFIELD, Zone::CAMP}, new_zone)) {
         if(same_pos_check()) {
            // the unit's old zone is the units new zone and it is already at the position in which
            // it is supposed to move or the battlefield is full
            return false;
         }
      }
      _remove_loc_tags(card);
      pos.index = index;
      m_registry.emplace< tag::location< new_zone > >(card);
   }

   /**
    * Counts the units in the camp or the battlefield, subject to a filter on
    * @param team Team,
    *   the team whose units should be counted
    * @param in_camp bool,
    *   whether to count the units in the cmap or on the battlefield
    * @param filter Callable,
    *   the filter to use on the units. Can be used to search for units with specific attributes
    * @return size_t,
    *   the count
    */
   template < Zone zone >
   [[nodiscard]] size_t count_if(
      Team team,
      const std::function< bool(entt::registry&, entt::entity) >& filter) const;

   [[nodiscard]] auto max_size_bf() const { return m_size_max_bf; }
   [[nodiscard]] auto max_size_camp() const { return m_size_max_camp; }

   template < Team::Value team >
   [[nodiscard]] std::vector< entt::entity > camp_units() const;
   [[nodiscard]] std::vector< entt::entity > camp_units(Team team) const;

   template < Zone zone >
   void add_to_queue(entt::entity card) = delete;
   template <>
   void add_to_queue< Zone::CAMP >(entt::entity card);
   template <>
   void add_to_queue< Zone::BATTLEFIELD >(entt::entity card);
   template < Zone zone >
   void add_to_queue(const std::vector< entt::entity >& units) = delete;
   template <>
   void add_to_queue< Zone::CAMP >(const std::vector< entt::entity >& units);
   template <>
   void add_to_queue< Zone::BATTLEFIELD >(const std::vector< entt::entity >& units);

  private:
   /// the max number of cards in the camp at any time
   size_t m_size_max_camp;
   /// the max number of units on the battlefield at any time
   size_t m_size_max_bf;
   // the ContainerType holding all entities
   std::array< size_t, n_zone > m_size_cache = {0};
   /// the queue for adding new cards to the camp
   SymArr< Queue > m_q_camp = {};
   // the queue for newly summoned units on the battlefield
   SymArr< Queue > m_q_bf = {};

   /// internal logic methods

   void _process_camp_queues(entt::registry& registry);

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
      m_size_cache[static_cast< size_t >(zone)] = c;
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
   void _remove_loc_tags(entt::entity card) { _remove_loc_tags_impl< Zone(n_zone - 1) >(card); }

   template < Zone zone >
   void _remove_loc_tags_impl(entt::entity card)
   {
      if(auto view = m_registry.view< tag::location< zone > >(); view.contains(card)) {
         m_registry.remove< tag::location< zone > >(card);
         _update_positions< zone >();
         return;
      }
      // if it wasn't this position, then we go on, to the remainder
      if constexpr(static_cast< size_t >(zone) > 0) {
         _remove_loc_tags_impl< static_cast< Zone >(static_cast< size_t >(zone) - 1) >(card);
      }
   }
};

template < Zone zone >
size_t BoardSystem::count_if(
   Team team,
   const std::function< bool(entt::registry&, entt::entity) >& filter) const
{
   size_t sum = 0;
   for(entt::entity elem : m_registry.view< tag::location< zone > >()) {
      if(filter(m_registry, elem)) {
         sum += 1;
      }
   }
   return sum;
}

template < Team::Value team >
std::vector< entt::entity > BoardSystem::camp_units() const
{
   auto view = m_registry.view< tag::location< Zone::CAMP >, tag::team< team >, tag::unit >();
   return std::vector< entt::entity >{view.begin(), view.end()};
}

template <>
void BoardSystem::add_to_queue< Zone::CAMP >(entt::entity card)
{
   m_q_camp[m_registry.get< Team >(card)].emplace(card);
}
template <>
void BoardSystem::add_to_queue< Zone::BATTLEFIELD >(entt::entity card)
{
   m_q_bf[m_registry.get< Team >(card)].emplace(card);
}
template <>
void BoardSystem::add_to_queue< Zone::CAMP >(const std::vector< entt::entity >& units)
{
   for(auto unit : units) {
      add_to_queue< Zone::CAMP >(unit);
   }
}
template <>
void BoardSystem::add_to_queue< Zone::BATTLEFIELD >(const std::vector< entt::entity >& units)
{
   for(auto unit : units) {
      add_to_queue< Zone::BATTLEFIELD >(unit);
   }
}

#endif  // LORAINE_BOARD_SYSTEM_H
