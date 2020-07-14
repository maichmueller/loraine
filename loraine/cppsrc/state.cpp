

#include "state.h"
#include "event/active_event.h"

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

void State::_commit_to_history(sptr< Action > action)
{
   m_history.emplace_back(std::move(action));
}
void State::_check_enlightenment() const
{
   auto player = PLAYER::BLUE;

   if(get_mana_gems(player) >= MAX_MANA) {
      events::active_event::set(events::EnlightenmentEvent(player));
   }
   player = PLAYER::RED;

   if(get_mana_gems(player) >= MAX_MANA) {
      events::active_event::set(events::EnlightenmentEvent(player));
   }
}
