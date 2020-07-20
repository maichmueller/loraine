
#ifndef LORAINE_GAME_H
#define LORAINE_GAME_H

#include "agent.h"
#include "cards/card.h"
#include "rulesets.h"
#include "state.h"
#include "types.h"

class Game {
   sptr< State > m_state;
   SymArr< Agent > m_agents;

   bool m_battle_mode = false;

   void _mulligan();
   void _draw_initial_hand(PLAYER player);
   void _do_action(const sptr< Action >& action);
   /*
    * Start the next round
    */
   void _start_round();

   void _end_round();

   void _activate_battlemode(bool b);

   void _resolve_battle();

   void _resolve_spell_stack();
  public:
   bool run_game();
};

#endif  // LORAINE_GAME_H
