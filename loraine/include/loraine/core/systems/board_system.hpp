
#ifndef LORAINE_BOARD_SYSTEM_HPP
#define LORAINE_BOARD_SYSTEM_HPP

#include <entt/entt.hpp>
#include <functional>
#include <iostream>
#include <queue>
#include <utility>
#include <variant>

#include "loraine/core/components.hpp"
#include "loraine/core/gamedefs.h"
#include "loraine/core/systems/system.hpp"
#include "loraine/utils/utils.h"

class BoardSystem: public ILogicSystem {
  public:
   using Queue = std::queue< entt::entity >;

   BoardSystem(uint camp_size, uint bf_size) : m_size_max_camp(camp_size), m_size_max_bf(bf_size) {}

   BoardSystem(
      GameState& state,
      uint camp_size,
      uint bf_size,
      SymArr< Queue > camp_queues,
      SymArr< Queue > bf_queues)
       : m_size_max_camp(camp_size),
         m_size_max_bf(bf_size),
         m_q_camp(std::move(camp_queues)),
         m_q_bf(std::move(bf_queues))
   {
   }

   /**
    * @brief Moves the card to the new zone at the given index.
    *
    * This method will update the position indices of the previously allocated zone of the card.
    * The Position component of each card should stay up-to-date this way for every move.
    */
   template < Zone new_zone >
   bool move_to(entt::entity card, uint index);
   template < Zone new_zone >
   bool move_to(const std::vector< entt::entity >& cards, const std::vector< uint >& indices);
   template < Zone new_zone >
   bool move_to(const std::vector< entt::entity >& cards);

   template < Zone zone >
   entt::entity at(uint index);

   template < Zone zone >
   size_t size()
   {
      return m_size_cache[static_cast< size_t >(zone)];
   }

   /**
    * Counts the units in the camp or the battlefield, subject to a filter on_add
    * @param team Team,
    *   the team whose units should be counted
    * @param in_camp bool,
    *   whether to count the units in the camp or on_add the battlefield
    * @param filter Callable,
    *   the filter to use on_add the units. Can be used to search for units with specific attributes
    * @return size_t,
    *   the count
    */
   template < Zone zone >
   [[nodiscard]] uint count_if(
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
      return m_registry->view< Position< zone > >().empty();
   }

  private:
   /// the max number of cards in the camp at any time
   uint m_size_max_camp;
   /// the max number of units on_add the battlefield at any time
   uint m_size_max_bf;
   // the ContainerType holding all entities
   std::array< uint, n_zone > m_size_cache = {0};
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
   void boundary_check(uint index) const
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
   uint _update_positions()
   {
      auto view = m_registry->view< Position< zone > >();
      std::vector< std::pair< entt::entity, uint > > card_pos;
      view.each([&](auto card, auto& pos) { card_pos.emplace_back({card, pos.index}); });

      std::sort(card_pos.begin(), card_pos.end(), [](const auto& p1, const auto& p2) {
         return p1.second < p2.second;
      });

      auto c = 0UL;
      auto updater = [&](Position< zone >& position) { position.index = c++; };
      for(auto [card, _] : card_pos) {
         m_registry->patch< Position< zone > >(card, updater);
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
      if(m_registry->view< Position< zone > >().contains(card)) {
         m_registry->remove< Position< zone > >(card);
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
uint BoardSystem::count_if(
   Team team,
   const std::function< bool(entt::registry&, entt::entity) >& filter) const
{
   uint sum = 0;
   for(auto elem : m_registry->view< Position< zone > >()) {
      if(filter(*m_registry, elem)) {
         sum += 1;
      }
   }
   return sum;
}

template < Team::Value team >
std::vector< entt::entity > BoardSystem::camp_units() const
{
   auto view = m_registry->view< Position< Zone::CAMP >, tag::team< team >, tag::unit >();
   return std::vector< entt::entity >{view.begin(), view.end()};
}

template <>
void BoardSystem::add_to_queue< Zone::CAMP >(entt::entity card)
{
   m_q_camp[m_registry->get< Team >(card)].emplace(card);
}

template <>
void BoardSystem::add_to_queue< Zone::BATTLEFIELD >(entt::entity card)
{
   m_q_bf[m_registry->get< Team >(card)].emplace(card);
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
bool BoardSystem::move_to(entt::entity card, uint index)
{
   if constexpr(Zone::BATTLEFIELD == new_zone || Zone::CAMP == new_zone) {
      auto same_pos_check = [&, this]() {
         return (m_registry->all_of< Position< new_zone > >(card)
                 && m_registry->get< Position< new_zone > >(card).index == index)
                || m_size_cache[static_cast< uint >(new_zone)] == [&]() {
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
   m_registry->emplace< Position< new_zone > >(card, index);
   return true;
}

template < Zone new_zone >
bool BoardSystem::move_to(
   const std::vector< entt::entity >& cards,
   const std::vector< uint >& indices)
{
   auto size = cards.size();
   if(size != indices.size()) {
      throw std::invalid_argument("Indices and cards vectors have different lengths.");
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
   std::vector< uint > indices;
   std::iota(indices.begin(), indices.end(), m_size_cache[static_cast< uint >(new_zone)]);
   return move_to< new_zone >(cards, indices);
}

template < Zone zone >
entt::entity BoardSystem::at(uint index)
{
   boundary_check< zone >(index);
   std::optional< entt::entity > e = std::nullopt;
   for(auto entity : m_registry->view< Position< zone > >()) {
      if(m_registry->get< Position< zone > >(entity).index == index) {
         e = entity;
         break;
      }
   }
   if(not utils::has_value(e)) {
      throw std::logic_error("The demanded index was not assigned to any entity.");
   }
   return e.value();
}

#endif  // LORAINE_BOARD_SYSTEM_HPP
