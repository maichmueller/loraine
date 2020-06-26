
#ifndef LORAINE_BOARD_H
#define LORAINE_BOARD_H

#include "cards/card.h"
#include "types.h"

class Board {
   // the container holding battling units
   SymArray< std::vector< Card > > battlefield;
   // the container holding summoned, active units
   SymArray< std::vector< Card > > backrow;

};

#endif  // LORAINE_BOARD_H
