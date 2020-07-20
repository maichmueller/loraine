
#ifndef LORAINE_BOARD_H
#define LORAINE_BOARD_H

#include <queue>

#include "cards/card.h"
#include "rulesets.h"
#include "types.h"

class Board {
   using OptCardPtr = std::optional< sptr< Card > >;
   // the container holding battling units
   SymArr< std::array< OptCardPtr, BATTLEFIELD_SIZE > > m_battlefield;
   SymArr< std::queue< OptCardPtr, std::vector< OptCardPtr > > >
      m_battlefield_queue;

   // the container holding summoned/played, alive units
   SymArr< std::array< OptCardPtr, BACKROW_SIZE > > m_backrow;
   SymArr< std::queue< OptCardPtr, std::vector< OptCardPtr > > >
      m_backrow_queue;

  public:
   Board()
       : m_battlefield(), m_battlefield_queue(), m_backrow(), m_backrow_queue()
   {
   }

   void move_to_battlefield(std::vector< size_t > attackers, PLAYER player);

   auto & get_battlefield(PLAYER player) {
      return m_battlefield[player];
   }
   auto & get_backrow(PLAYER player) {
      return m_backrow[player];
   }
};


#endif  // LORAINE_BOARD_H
