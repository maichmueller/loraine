
#ifndef LORAINE_GAME_H
#define LORAINE_GAME_H

#include "agent.h"
#include "cards/card.h"
#include "rulesets.h"
#include "state.h"
#include "types.h"

class Game {
   sptr< State > m_state;
   SymArr< Agent > agents;

   void _mulligan();
   void _draw_initial_hand(PLAYER player);
   void _do_action(const sptr< Action >& action);
   /*
    * Start the next round
    */
   void _start_round();

   void _end_round();

  public:
   bool run_game();
};

#endif  // LORAINE_GAME_H
