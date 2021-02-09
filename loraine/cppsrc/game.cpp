
#include "game.h"

#include <algorithms.h>

#include <utility>

#include "grants/grant.h"
#include "rulesets.h"
#include "utils.h"


bool Game::run_game()
{
   // decide the player who starts attacking
   Player starting_player = Player(
      std::uniform_int_distribution< size_t >(0, 1)(rng::get_engine()));
   m_state->set_starting_player(starting_player);

   // draw the starting cards for each player and let them _mulligan
   auto hand_blue = _draw_n_cards(BLUE, INITIAL_HAND_SIZE);
   auto hand_red = _draw_n_cards(RED, INITIAL_HAND_SIZE);
   _mulligan(hand_blue, hand_red);

   // the actual run of the game. We play until the state says it is in a
   // terminal state.

   while(true) {
      // ask current agent for action
      Player turn = m_state->get_active_player();
      auto action = m_agents[turn]->decide_action(*m_state);

      if(_do_action(action)) {
         m_state->incr_turn();
      }

      if(auto terminality = m_state->is_terminal() != ONGOING) {
         return terminality;
      }
   }
}
