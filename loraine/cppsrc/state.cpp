

#include "state.h"

int State::start_round()
{
   return 0;
}
sptr< Card > State::draw_card()
{
   return sptr< Card >();
}

void State::_check_terminal()
{
   if(m_round > MAX_ROUNDS) {
      m_terminal = TIE;
   }
   if(m_nexus_health[PLAYER::BLUE] < 1) {
      if(m_nexus_health[PLAYER::RED] < 1) {
         m_terminal = TIE;
      }
      m_terminal = RED_WINS_NEXUS;
   }
   if(m_nexus_health[PLAYER::RED] < 1) {
      if(m_nexus_health[PLAYER::BLUE] < 1) {
         m_terminal = TIE;
      }
      m_terminal = BLUE_WINS_NEXUS;
   }
   m_terminal = ONGOING;
   m_terminal_checked = true;
}
