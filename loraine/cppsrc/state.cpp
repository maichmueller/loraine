

#include "state.h"

#include "action.h"
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

void State::_commit_to_history(sptr< AnyAction > action)
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
sptr< Card > State::draw_card_by_idx(PLAYER player, size_t index)
{
   return m_deck_cont[player].draw_card_by_index(index);
}
std::vector< sptr< Card > > State::draw_n_cards(
   PLAYER player, size_t n, bool random)
{
   std::vector< sptr< Card > > cards;
   cards.reserve(n);

   if(random) {
      auto deck = m_deck_cont.at(player);

      std::vector< size_t > indices;
      indices.reserve(deck.size());
      for(size_t i = 0; i < deck.size(); ++i) {
         indices.at(i) = i;
      }

      // draw without replacement n many indices by shuffling
      // all indices and taking the first n many.
      std::shuffle(indices.begin(), indices.end(), rng::rng_def_engine);
      for(size_t i = 0; i < n; ++i) {
         cards.emplace_back(draw_card_by_idx(player, indices.at(i)));
      }
   } else {
      for(size_t i = 0; i < n; ++i) {
         cards.emplace_back(draw_card());
      }
   }
   return cards;
}
