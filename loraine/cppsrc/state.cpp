

#include "state.h"

#include <algorithms.h>

#include "action.h"

void State::_check_terminal()
{
   if(m_round > MAX_ROUNDS) {
      m_terminal = TIE;
   }
   if(m_nexus_health[BLUE] < 1) {
      if(m_nexus_health[RED] < 1) {
         m_terminal = TIE;
      }
      m_terminal = RED_WINS_NEXUS;
   }
   if(m_nexus_health[RED] < 1) {
      if(m_nexus_health[BLUE] < 1) {
         m_terminal = TIE;
      }
      m_terminal = BLUE_WINS_NEXUS;
   }
   m_terminal = ONGOING;
   m_terminal_checked = true;
}

void State::commit_to_history(sptr< Action > action)
{
   m_history[action->get_player()][action->get_round()].emplace_back(std::move(action));
}

void State::to_graveyard(const sptr< Card >& unit)
{
   m_graveyard.at(unit->mutables().owner).at(m_round).emplace_back(unit);
}
void State::to_tossed(const sptr< Card >& card)
{
   m_tossed_cards.at(card->mutables().owner).emplace_back(card);
}
State::State(
   Player starting_player,
   SymArr< HandType > hands,
   SymArr< DeckType > decks,
   sptr< Board > board,
   SymArr< long > nexus_health,
   SymArr< size_t > mana,
   SymArr< size_t > managems,
   SymArr< size_t > spell_mana,
   SymArr< bool > can_attack,
   SymArr< bool > scout_token,
   SymArr< bool > can_plunder,
   SymArr< std::map< size_t, std::vector< sptr< Card > > > > graveyard,
   SymArr< std::vector< sptr< Card > > > tossed_cards,
   SymArr< std::map< size_t, std::vector< sptr< Action > > > > history,
   std::optional< Player > attacker,
   bool battle_mode,
   size_t round,
   Player turn,
   SymArr< bool > passes,
   Status terminal,
   bool terminal_checked,
   SpellStackType spell_stack,
   SpellStackType spell_prestack)
    : m_nexus_health(nexus_health),
      m_mana(mana),
      m_managems(managems),
      m_spell_mana(spell_mana),
      m_can_attack(can_attack),
      m_scout_token(scout_token),
      m_can_plunder(can_plunder),
      m_hand(std::move(hands)),
      m_deck_cont(std::move(decks)),
      m_graveyard(std::move(graveyard)),
      m_tossed_cards(std::move(tossed_cards)),
      m_board(std::move(board)),
      m_history(std::move(history)),
      m_starting_player(starting_player),
      m_attacker(attacker),
      m_battle_mode(battle_mode),
      m_round(round),
      m_turn(turn),
      m_passed(passes),
      m_terminal(terminal),
      m_terminal_checked(terminal_checked),
      m_spell_stack(std::move(spell_stack)),
      m_spell_prestack(std::move(spell_prestack))
{
}
std::tuple< Location, long > State::find(const sptr< Card >& card) const
{
   auto location = card->mutables().location;
   long index = 0;
   if(location == Location::BATTLEFIELD) {
      index = algo::find_index(m_board->get_battlefield(card->mutables().owner), card);
   } else if(location == Location::CAMP) {
      index = algo::find_index(m_board->get_camp(card->mutables().owner), card);
   } else if(location == Location::HAND) {
      index = algo::find_index(hand(card->mutables().owner), card);
   } else if(location == Location::DECK) {
      index = algo::find_index(deck(card->mutables().owner), card);
   }
   return {location, index};
}
