
#include "core/gamemode.h"

#include <algorithms.h>

#include <utility>

#include "core/gamedefs.h"
#include "grants/grant.h"
#include "utils/utils.h"

bool GameMode::run_game()
{
   while(true) {
      // ask current player for action
      auto action = m_state->player(m_state->active_team()).controller()->choose_action(*m_state);

      if(_do_action(action)) {
         m_state->incr_turn();
      }

      if(auto terminality = m_state->status() != ONGOING) {
         return terminality;
      }
   }
}
