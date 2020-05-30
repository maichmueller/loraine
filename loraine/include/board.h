//
// Created by michael on 30.05.20.
//

#ifndef LORAINE_BOARD_H
#define LORAINE_BOARD_H

#include "card.h"
#include "types.h"

class Board {
   // the container holding battling units
   SymArray< std::vector< Card > > battlefield;
   // the container holding summoned, active units
   SymArray< std::vector< Card > > camp;

};

#endif  // LORAINE_BOARD_H
