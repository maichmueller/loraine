
#include "game.h"

bool Game::run_game()
{
   // decide the player who starts attacking
   unsigned short starting_player = std::uniform_int_distribution< size_t >(
      0, 1)(rng::rng_def_engine);
   m_state->set_starting_player(PLAYER(starting_player));

   // draw the starting cards for each player and let them _mulligan
   _draw_initial_hand(PLAYER::BLUE);
   _draw_initial_hand(PLAYER::RED);
   _mulligan();

   // the actual run of the game. We play until the state says it is in a
   // terminal state. Then we return out out of the method
   while(true) {
      _start_round();

      while(true) {
         // check for breaking the action loop
         if(const auto& hist_ptr = *m_state->get_history();
            hist_ptr.size() > 1  // if we didnt't check this we'd get a segfault
            && (*(std::rbegin(hist_ptr)))->get_action_type()
                  == ActionType::PASS  // the ultimate action was a pass
            && (*(std::rbegin(hist_ptr) + 1))->get_action_type()
                  == ActionType::PASS) {  // the penultimate action was a pass

            // both players passed, therefore the round is over
            break;
         }

         // ask current agent for action
         PLAYER turn = m_state->get_turn();
         auto action = agents[turn].decide_action(*m_state);
         m_state->do_action(action);
      }

      if(auto terminality = m_state->is_terminal() != ONGOING) {
         return terminality;
      }
   }
}
