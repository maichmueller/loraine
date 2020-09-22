
#ifndef LORAINE_BOARD_H
#define LORAINE_BOARD_H

#include <queue>

#include "cards/card.h"
#include "cards/grants.h"
#include "rulesets.h"
#include "types.h"

class Board {
   using OptCardPtr = std::optional< sptr< Unit > >;
   // the container holding battling units
   SymArr< std::array< OptCardPtr, BATTLEFIELD_SIZE > > m_battlefield;
   SymArr< std::queue< OptCardPtr, std::vector< OptCardPtr > > >
      m_battlefield_queue;

   // the container holding summoned/played, alive units
   SymArr< std::array< OptCardPtr, CAMP_SIZE > > m_camp;
   SymArr< std::queue< OptCardPtr, std::vector< OptCardPtr > > >
      m_backrow_queue;

  public:
   void move_to_battlefield(
      const std::vector< size_t >& field_positions, PLAYER player);
   void move_to_battlefield(
      const std::map< size_t, size_t >& units, PLAYER player);

   void retreat_to_camp(PLAYER player);

   auto& get_battlefield(PLAYER player) { return m_battlefield[player]; }
   auto& get_camp(PLAYER player) { return m_camp[player]; }

   void reorganize_camp(PLAYER player, size_t start_from_idx = 0);
};

#endif  // LORAINE_BOARD_H
