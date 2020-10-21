

#include "state.h"

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

void State::commit_to_history(sptr< AnyAction > action)
{
   m_history[action->get_player()][action->get_round()].emplace_back(std::move(action));
}

sptr< Card > State::draw_card_by_idx(Player player, size_t index)
{
   return m_deck_cont[player].draw_card_by_index(index);
}
std::vector< sptr< Card > > State::draw_n_cards(Player player, size_t n, bool random)
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
void State::move_to_graveyard(sptr< Unit > unit)
{
   auto player = unit->get_owner();
   m_graveyard.at(player).at(m_round).emplace_back(unit);
   m_board->remove_dead_units({unit});
}
State::State(
   Player starting_player,
   SymArr< State::HandType > hands,
   SymArr< DeckContainer > decks,
   sptr< Board > board,
   SymArr< long > nexus_health,
   SymArr< size_t > mana,
   SymArr< size_t > managems,
   SymArr< size_t > spell_mana,
   SymArr< bool > can_attack,
   SymArr< bool > scout_token,
   SymArr< bool > can_plunder,
   SymArr< std::map< size_t, std::vector< sptr< Unit > > > > graveyard,
   SymArr< std::vector< sptr< Card > > > tossed_cards,
   SymArr< std::map< size_t, std::vector< sptr< AnyAction > > > > history,
   std::optional< Player > attacker,
   size_t round,
   Player turn,
   unsigned short pass_count,
   Status terminal,
   bool terminal_checked,
   std::vector< sptr< Spell > > spell_stack)
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
      m_round(round),
      m_turn(turn),
      m_can_endround(pass_count),
      m_terminal(terminal),
      m_terminal_checked(terminal_checked),
      m_spell_stack(std::move(spell_stack))
{
}
