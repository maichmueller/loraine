
#include "core/gamemode.h"

#include <algorithms.h>

#include <utility>

#include "core/gamedefs.h"
#include "grants/grant.h"
#include "utils/utils.h"
#include "core/logic.h"

bool GameMode::run_game()
{
   const Logic * logic = m_state.logic();
   while(true) {
      bool flip_initiative = false;

      if(auto* targ_buffer = m_state.targeting_buffer(); targ_buffer->empty()) {
         // if no targets are currently required, then simply ask the current player for action
         auto action = logic->request_action();
      } else {
         // the targeting buffer is currently not empty so we demand targets
         m_state.logic()->transition_action_handler< TargetModeHandler >();
         while(not targ_buffer->empty()) {
            auto targeting_action = logic->request_targets().execute(m_state);
         }
         logic->restore_previous_handler();
      }
      if(logic->is_valid(action)) {
         flip_initiative = action.execute(m_state);
      }
      if(_do_action(action)) {
         m_state->incr_turn();
      }

      if(auto terminality = m_state->status() != ONGOING) {
         return terminality;
      }
   }
}
