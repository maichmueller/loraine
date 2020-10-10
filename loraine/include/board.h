
#ifndef LORAINE_BOARD_H
#define LORAINE_BOARD_H

#include <queue>
#include <utility>

#include "cards/card.h"
#include "cards/grant.h"
#include "rulesets.h"
#include "types.h"

class Board {
   using Battlefield = std::
      array< std::optional< sptr< Unit > >, BATTLEFIELD_SIZE >;
   using Camp = std::vector< sptr< Unit > >;
   using CampQueue = std::queue< sptr< Unit > >;

   // the container holding battling units
   SymArr< Battlefield > m_battlefield;

   // the container holding summoned/played, alive units
   SymArr< Camp > m_camp;
   SymArr< CampQueue > m_camp_queue;

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
   Board(
      SymArr< Battlefield > bfs,
      SymArr< Camp > camps,
      SymArr< CampQueue > queues)
       : m_battlefield(std::move(bfs)),
         m_camp(std::move(camps)),
         m_camp_queue(std::move(queues))
   {
      _reserve_space();
   }

   void move_to_battlefield(
      const std::vector< size_t >& field_positions, Player player);
   void move_to_battlefield(
      const std::map< size_t, size_t >& units, Player player);

   std::pair< bool, Camp::iterator > find_in_camp(const sptr< Unit >& unit);
   std::pair< bool, Battlefield ::iterator > find_on_battlefield(
      const sptr< Unit >& unit);

   void remove_dead_units(const std::vector< sptr< Unit > >& units);
   void remove_dead_units(
      Player player, std::vector< size_t > indices, bool in_camp);

   /*
    * Counts the units in the camp or the battlefield, subject to a filter on
    * the unit ptr, if desired.
    *
    */
   [[nodiscard]] size_t count_units(
      Player player,
      bool in_camp,
      const std::function< bool(const sptr< Unit >&) >& filter) const;

   [[nodiscard]] size_t count_units(Player player, bool in_camp) const;

   auto& get_battlefield(Player player) { return m_battlefield[player]; }
   [[nodiscard]] auto& get_battlefield(Player player) const
   {
      return m_battlefield[player];
   }
   auto& get_camp(Player player) { return m_camp[player]; }
   [[nodiscard]] auto& get_camp(Player player) const { return m_camp[player]; }
   auto& get_camp_queue(Player player) { return m_camp_queue.at(player); }
   [[nodiscard]] auto& get_camp_queue(Player player) const
   {
      return m_camp_queue.at(player);
   }

   inline void add_to_queue(const sptr< Unit >& unit)
   {
      m_camp_queue[unit->get_owner()].emplace(unit);
   }
   inline void add_to_queue(std::vector< sptr< Unit > > units)
   {
      for(auto&& unit : units) {
         m_camp_queue[unit->get_owner()].emplace(std::move(unit));
      }
   }
};

#endif  // LORAINE_BOARD_H
