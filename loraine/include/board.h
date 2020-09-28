
#ifndef LORAINE_BOARD_H
#define LORAINE_BOARD_H

#include <queue>

#include "cards/card.h"
#include "cards/grants.h"
#include "rulesets.h"
#include "types.h"

class Board {
   using OptCardPtr = std::optional< sptr< Unit > >;
   using Battlefield = std::array< OptCardPtr, BATTLEFIELD_SIZE >;
   using Camp = std::array< OptCardPtr, CAMP_SIZE >;
   using CampQueue = std::queue< OptCardPtr, std::vector< OptCardPtr > >;

   // the container holding battling units
   SymArr< Battlefield > m_battlefield;

   // the container holding summoned/played, alive units
   SymArr< Camp > m_camp;
   SymArr< CampQueue > m_camp_queue;

  public:
   void move_to_battlefield(
      const std::vector< size_t >& field_positions, Player player);
   void move_to_battlefield(
      const std::map< size_t, size_t >& units, Player player);

   std::pair< bool, Camp::iterator > find_in_camp(const sptr< Unit >& unit);
   std::pair< bool, Battlefield ::iterator > find_on_battlefield(
      const sptr< Unit >& unit);

   void remove_dead_unit(const sptr< Unit >& unit);

   /*
    * Counts the units in the camp or the battlefield, subject to a filter on
    * the unit ptr, if desired.
    *
    */
   size_t count_units(
      Player player,
      bool in_camp,
      const std::function< bool(const sptr< Unit >&) >& filter =
         [](const sptr< Unit >& /*unused*/) { return true; });

   auto& get_battlefield(Player player) { return m_battlefield[player]; }
   auto& get_camp(Player player) { return m_camp[player]; }
   auto& get_camp_queue(Player player) { return m_camp_queue.at(player); }

   void reorganize_camp(Player player, size_t start_from_idx = 0);

   inline void add_to_queue(sptr< Unit > unit)
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
