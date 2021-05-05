
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

   // TODO: make this update the correct previous zone of the moved entity too!
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
   template < Zone new_zone >
   bool move_to(entt::entity card, size_t index);
   template < Zone new_zone >
   bool move_to(const std::vector< entt::entity >& cards, const std::vector< size_t >& indices);
   template < Zone new_zone >
   bool move_to(const std::vector< entt::entity >& cards);

   template < Zone zone >
   entt::entity at(size_t index);

   template < Zone zone >
   size_t size()
   {
      return m_size_cache[static_cast< size_t >(zone)];
   }

   /**
    * Counts the units in the camp or the battlefield, subject to a filter on
    * @param team Team,
    *   the team whose units should be counted
    * @param in_camp bool,
    *   whether to count the units in the camp or on the battlefield
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
   /// these specializations should be fine according to C++17 std, but GCC complains
   //   template <>
   //   void add_to_queue< Zone::CAMP >(entt::entity card);
   //   template <>
   //   void add_to_queue< Zone::BATTLEFIELD >(entt::entity card);
   template < Zone zone >
   void add_to_queue(const std::vector< entt::entity >& units) = delete;
   /// these specializations should be fine according to C++17 std, but GCC complains
   //   template <>
   //   void add_to_queue< Zone::CAMP >(const std::vector< entt::entity >& units);
   //   template <>
   //   void add_to_queue< Zone::BATTLEFIELD >(const std::vector< entt::entity >& units);

   template < Zone zone >
   inline bool empty() const
   {
      return m_registry.view< Position< zone > >().empty();
   }

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

   template <
      Zone zone,
      typename = std::enable_if_t< zone == Zone::CAMP || zone == Zone::BATTLEFIELD > >
   void _process_queue();

   template < Zone zone >
   void boundary_check(size_t index) const
   {
      if constexpr(Zone::CAMP == zone) {
         if(not (index < m_size_max_camp)) {
            throw std::out_of_range(
               "An idex that is greater than the maximum camp size was demanded.");
         }
      } else if constexpr(Zone::BATTLEFIELD == zone) {
         if(not (index < m_size_max_bf)) {
            throw std::out_of_range(
               "An idex that is greater than the maximum battlefield size was demanded.");
         }
      }
   }

   /**
    * @brief Reindexes the entities in a location after a removal.
    *
    * @tparam zone
    * @return
    */
   template < Zone zone >
   size_t _update_positions()
   {
      std::vector< Position< zone >* > positions;
      m_registry.view< Position< zone > >().each(
         [&](auto& pos) { positions.emplace_back(&pos); });

      std::sort(positions.begin(), positions.end(), [](const auto* p1, const auto* p2) {
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
   void _remove_pos(entt::entity card) { _remove_pos_impl< Zone(n_zone - 1) >(card); }

   template < Zone zone >
   void _remove_pos_impl(entt::entity card)
   {
      if(m_registry.view< Position< zone > >().contains(card)) {
         m_registry.remove< Position< zone > >(card);
         _update_positions< zone >();
         return;
      }
      // if it wasn't this position, then we go on, to the remainder
      if constexpr(static_cast< size_t >(zone) > 0) {
         _remove_pos_impl< static_cast< Zone >(static_cast< size_t >(zone) - 1) >(card);
      }
   }
};

template < Zone zone >
size_t BoardSystem::count_if(
   Team team,
   const std::function< bool(entt::registry&, entt::entity) >& filter) const
{
   size_t sum = 0;
   for(auto elem : m_registry.view< Position< zone > >()) {
      if(filter(m_registry, elem)) {
         sum += 1;
      }
   }
   return sum;
}

template < Team::Value team >
std::vector< entt::entity > BoardSystem::camp_units() const
{
   auto view = m_registry.view< Position< Zone::CAMP >, tag::team< team >, tag::unit >();
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

template < Zone new_zone >
bool BoardSystem::move_to(entt::entity card, size_t index)
{
   if constexpr(algo::contains(std::array{Zone::BATTLEFIELD, Zone::CAMP}, new_zone)) {
      auto same_pos_check = [&, this]() {
         return (m_registry.all_of< Position< new_zone > >(card)
                 && m_registry.get< Position< new_zone > >(card).index == index)
                || m_size_cache[static_cast< size_t >(new_zone)] == [&]() {
                      if constexpr(new_zone == Zone::BATTLEFIELD) {
                         return m_size_max_bf;
                      }
                      return m_size_max_camp;
                   }();
      };
      if(same_pos_check()) {
         // the unit's old zone is the units new zone and it is already at the position in which
         // it is supposed to move or the battlefield is full
         return false;
      }
   }
   _remove_pos(card);
   m_registry.emplace< Position< new_zone > >(card, index);
   return true;
}

template < Zone new_zone >
bool BoardSystem::move_to(
   const std::vector< entt::entity >& cards,
   const std::vector< size_t >& indices)
{
   auto size = cards.size();
   if(size != indices.size()) {
      throw std::invalid_argument("Indices and cards vector have different lengths.");
   }
   std::vector< bool > out(size);
   for(auto i = 0; i < size; ++i) {
      out.emplace_back(move_to< new_zone >(cards[i], indices[i]));
   }
   return std::accumulate(out.begin(), out.end(), 0) == size;
}

template < Zone new_zone >
bool BoardSystem::move_to(const std::vector< entt::entity >& cards)
{
   std::vector< size_t > indices;
   std::iota(indices.begin(), indices.end(), m_size_cache[static_cast< size_t >(new_zone)]);
   return move_to< new_zone >(cards, indices);
}

template < Zone zone >
entt::entity BoardSystem::at(size_t index)
{
   boundary_check< zone >(index);
   std::optional< entt::entity > e = std::nullopt;
   for(auto entity : m_registry.view< Position< zone > >()) {
      if(m_registry.get< Position< zone > >(entity).index == index) {
         e = entity;
         break;
      }
   }
   if(not utils::has_value(e)) {
      throw std::logic_error("The demanded index was not assigned to any entity.");
   }
   return e.value();
}

#endif  // LORAINE_BOARD_SYSTEM_H
