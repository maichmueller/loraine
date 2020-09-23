
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
   using BattlefieldQueue = std::queue< OptCardPtr, std::vector< OptCardPtr > >;
   using Camp = std::array< OptCardPtr, CAMP_SIZE >;
   using CampQueue = std::queue< OptCardPtr, std::vector< OptCardPtr > >;

   // the container holding battling units
   SymArr< Battlefield > m_battlefield;
   SymArr< BattlefieldQueue > m_battlefield_queue;

   // the container holding summoned/played, alive units
   SymArr< Camp > m_camp;
   SymArr< CampQueue > m_camp_queue;

  public:
   void move_to_battlefield(
      const std::vector< size_t >& field_positions, PLAYER player);
   void move_to_battlefield(
      const std::map< size_t, size_t >& units, PLAYER player);

   void retreat_to_camp(PLAYER player);

   std::pair< bool, Camp::iterator > find_in_camp(const sptr< Unit >& unit);
   std::pair< bool, Battlefield ::iterator > find_on_battlefield(
      const sptr< Unit >& unit);

   void remove_dead_unit(const sptr< Unit >& unit);

   /*
    * Counts the units in the camp or the battlefield.
    *
    * Assumes that the relevant container is organized, i.e. all units present
    * are stored without empty slots in between them!
    */
   size_t count_camp(PLAYER player, bool in_camp);

   auto& get_battlefield(PLAYER player) { return m_battlefield[player]; }
   auto& get_camp(PLAYER player) { return m_camp[player]; }

   void reorganize_camp(PLAYER player, size_t start_from_idx = 0);
};

#endif  // LORAINE_BOARD_H
