
#ifndef LORAINE_STATE_H
#define LORAINE_STATE_H

#include <grants/grantfactory.h>

#include <array>
#include <stack>
#include <utility>
#include <vector>

#include "board.h"
#include "deck.h"
#include "events/event_listener.h"
#include "rng_machine.h"
#include "rulesets.h"
#include "types.h"

class Card;
class Action;
class Game;

class State {
   friend Game;

  public:
   using HandType = std::vector< sptr< Card > >;
   using DeckType = std::vector< sptr< Card > >;
   using SpellStackType = std::vector< sptr< Spell > >;

   State(
      Player starting_player,
      SymArr< HandType > hands,
      SymArr< DeckType > decks,
      sptr< Board > board)
       : m_hand(std::move(hands)),
         m_deck_cont(std::move(decks)),
         m_board(std::move(board)),
         m_starting_player(starting_player),
         m_attacker(starting_player),
         m_turn(starting_player),
         m_spell_stack()
   {
   }
   State(
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
      SpellStackType spell_stack = {},
      SpellStackType spell_prestack = {});

   auto& events() { return m_events; }
   auto& events() const { return m_events; }

   inline void nexus_health(long int value, Player player) { m_nexus_health[player] = value; }
   [[nodiscard]] inline auto nexus_health(Player player) const { return m_nexus_health[player]; }
   inline void damage_nexus(size_t amount, Player player) { m_nexus_health[player] -= amount; }
   inline void heal_nexus(size_t amount, Player player)
   {
      m_nexus_health[player] = std::min(
         m_nexus_health[player] + long(amount), long(START_NEXUS_HEALTH));
   }

   inline void mana(size_t value, Player player)
   {
      m_mana[player] = std::max(std::min(MAX_MANA, value), size_t(0));
   }
   [[nodiscard]] inline auto mana(Player player) const { return m_mana[player]; }

   inline void managems(size_t value, Player player) { m_managems[player] = value; }
   [[nodiscard]] inline auto managems(Player player) const { return m_managems[player]; }

   inline void spell_mana(size_t value, Player player)
   {
      m_spell_mana[player] = std::max(
         std::min(static_cast< size_t >(MAX_SPELL_MANA), value), size_t(0));
   }
   [[nodiscard]] inline auto spell_mana(Player player) const { return m_spell_mana[player]; }

   inline void hand(HandType hand, Player player) { m_hand[player] = std::move(hand); }
   [[nodiscard]] inline auto& hand(Player player) const { return m_hand[player]; }
   [[nodiscard]] inline auto& hand(Player player) { return m_hand[player]; }

   inline void deck(DeckType deck, Player player) { m_deck_cont[player] = std::move(deck); }
   [[nodiscard]] inline auto& deck(Player player) const { return m_deck_cont[player]; }
   [[nodiscard]] inline auto& deck(Player player) { return m_deck_cont[player]; }

   inline void token_attack(bool value, Player player) { m_can_attack[player] = value; }
   [[nodiscard]] inline auto token_attack(Player player) const { return m_can_attack[player]; }
   inline void token_plunder(bool value, Player player) { m_can_plunder[player] = value; }
   [[nodiscard]] inline auto token_plunder(Player player) const { return m_can_plunder[player]; }
   inline void token_scout(Player player, bool value) { m_scout_token.at(player) = value; }
   [[nodiscard]] inline auto token_scout(Player player) const { return m_scout_token[player]; }

   [[nodiscard]] inline auto graveyard(Player player) const { return m_graveyard[player]; }
   [[nodiscard]] inline auto tossed_cards(Player player) const { return m_tossed_cards[player]; }
   [[nodiscard]] inline auto board() const { return m_board; }
   [[nodiscard]] inline auto* history() const { return &m_history; }
   [[nodiscard]] inline auto round() const { return m_round; }

   inline void turn(size_t turn) { m_turn = turn; }
   [[nodiscard]] inline auto turn() const { return m_turn; }

   inline void attacker(Player player) { m_attacker = player; }
   [[nodiscard]] inline auto attacker() const { return m_attacker; }
   inline void reset_attacker() { m_attacker.reset(); }

   inline void starting_player(Player player)
   {
      m_starting_player = player;
      token_attack(true, player);
      turn(player);
   }
   [[nodiscard]] inline auto starting_player() const { return m_starting_player; }
   [[nodiscard]] inline auto active_player() const { return Player(m_turn % 2); }

   [[nodiscard]] inline auto is_enlightened(Player player) const
   {
      return m_managems[player] == MAX_MANA;
   }

   [[nodiscard]] inline auto& spell_stack() { return m_spell_stack; }
   [[nodiscard]] inline auto spell_stack() const { return m_spell_stack; }
   [[nodiscard]] inline auto& spell_prestack() { return m_spell_prestack; }
   [[nodiscard]] inline auto spell_prestack() const { return m_spell_prestack; }
   inline void incr_managems(Player player, size_t amount = 1)
   {
      managems(std::min(m_managems[player] + amount, MAX_MANA), player);
   }

   [[nodiscard]] inline bool round_ended() const
   {
      return m_passed[Player::BLUE] && m_passed[Player::RED];
   }
   inline bool pass()
   {
      m_passed[active_player()] = true;
      return round_ended();
   }
   inline void reset_pass(Player player) { m_passed[player] = false; }
   inline void reset_pass() { m_passed[active_player()] = false; }
   inline void reset_pass_all() { m_passed = {false, false}; }
   inline void fill_mana(Player player, size_t amount) { m_mana[player] += amount; }
   inline void fill_mana(Player player) { m_mana[player] = m_managems[player]; }

   /*
    * Check if the current game state is terminal
    */
   inline int is_terminal()
   {
      if(not m_terminal_checked) {
         _check_terminal();
      }
      return m_terminal;
   }

   void commit_to_history(sptr< Action > action);

   void to_graveyard(const sptr< Card >& unit);
   void to_tossed(const sptr< Card >& card);

   [[nodiscard]] std::tuple< Location, long > find(const sptr< Card >& card) const;

  private:
   // player symmetric attributes
   SymArr< Player > nexus_ids{BLUE, RED};
   SymArr< long > m_nexus_health{START_NEXUS_HEALTH, START_NEXUS_HEALTH};
   SymArr< size_t > m_mana{0, 0};
   SymArr< size_t > m_managems{0, 0};
   SymArr< size_t > m_spell_mana{0, 0};

   SymArr< bool > m_can_attack{false, false};
   SymArr< bool > m_scout_token{false, false};
   SymArr< bool > m_can_plunder{false, false};
   SymArr< bool > m_passed{false, false};

   std::array<uptr< EventBase<Effect> >, events::n_events> m_events;

   SymArr< HandType > m_hand;
   SymArr< DeckType > m_deck_cont;
   SymArr< GrantFactory > m_grant_factory;
   SymArr< std::map< size_t, std::vector< sptr< Card > > > > m_graveyard{};
   SymArr< std::vector< sptr< Card > > > m_tossed_cards{};
   SymArr< std::map< size_t, std::vector< sptr< Action > > > > m_history;

   sptr< Board > m_board = std::make_shared< Board >();

   // state attributes
   Player m_starting_player;
   std::optional< Player > m_attacker;
   bool m_battle_mode = false;
   bool m_battle_resolution_mode = false;
   size_t m_round = 0;
   size_t m_turn;
   Status m_terminal = Status::ONGOING;
   bool m_terminal_checked = false;
   SpellStackType m_spell_stack;
   SpellStackType m_spell_prestack = {};

   void _check_terminal();
};

#endif  // LORAINE_STATE_H
