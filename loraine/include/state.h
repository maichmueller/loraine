
#ifndef LORAINE_STATE_H
#define LORAINE_STATE_H

#include <array>
#include <stack>
#include <utility>
#include <vector>

#include "action.h"
#include "board.h"
#include "cards/card.h"
#include "deck.h"
#include "rng_machine.h"
#include "rulesets.h"
#include "types.h"

class State {
  public:
   using HandType = std::vector< sptr< Card > >;

  private:
   // player symmetric attributes

   SymArr< Player > m_nexus_ids{BLUE, RED};
   SymArr< int > m_nexus_health{START_NEXUS_HEALTH, START_NEXUS_HEALTH};
   SymArr< size_t > m_mana{0, 0};
   SymArr< size_t > m_managems{0, 0};
   SymArr< size_t > m_spell_mana{0, 0};

   SymArr< bool > m_can_attack{false, false};
   SymArr< bool > m_scout_token{false, false};
   SymArr< bool > m_can_plunder{false, false};

   // the cards in hand (short: the player's 'hand')
   SymArr< HandType > m_hand;
   SymArr< DeckContainer > m_deck_cont;

   SymArr< std::map< size_t, std::vector< sptr< Unit > > > > m_graveyard{};
   SymArr< std::vector< sptr< Card > > > m_tossed_cards{};
   sptr< Board > m_board = std::make_shared< Board >();

   std::map< size_t, std::vector< sptr< AnyAction > > > m_history;

   // state attributes
   Player m_starting_player;
   std::optional< Player > m_attacker;
   size_t m_round = 0;
   Player m_turn;
   unsigned short m_pass_count = 0;
   Status m_terminal = Status::ONGOING;
   bool m_terminal_checked = false;
   std::vector< sptr< Spell > > m_spell_stack;

   void _check_terminal();

  public:
   State(
      Player starting_player,
      SymArr< HandType > hands,
      SymArr< DeckContainer > decks,
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
      SymArr< DeckContainer > decks,
      sptr< Board > board,
      SymArr< int > nexus_health,
      SymArr< size_t > mana,
      SymArr< size_t > managems,
      SymArr< size_t > spell_mana,
      SymArr< bool > can_attack,
      SymArr< bool > scout_token,
      SymArr< bool > can_plunder,
      SymArr< std::map< size_t, std::vector< sptr< Unit > > > > graveyard,
      SymArr< std::vector< sptr< Card > > > tossed_cards,
      std::map< size_t, std::vector< sptr< AnyAction > > > history,
      std::optional< Player > attacker,
      size_t round,
      Player turn,
      unsigned short pass_count,
      size_t terminal,
      bool terminal_checked,
      std::vector< sptr< Spell > > spell_stack);
   inline void set_nexus_health(long int value, Player player)
   {
      m_nexus_health[player] = value;
   }
   inline void damage_nexus(size_t amount, Player player)
   {
      m_nexus_health[player] -= amount;
   }
   inline void heal_nexus(size_t amount, Player player)
   {
      m_nexus_health[player] = std::min(
         m_nexus_health[player] + amount, size_t(START_NEXUS_HEALTH));
   }
   inline void set_mana(size_t value, Player player)
   {
      m_mana[player] = std::max(
         std::min(static_cast< size_t >(MAX_MANA), value), size_t(0));
   }

   inline void set_mana_gems(size_t value, Player player)
   {
      m_managems[player] = value;
   }
   inline void set_spell_mana(size_t value, Player player)
   {
      m_spell_mana[player] = std::max(
         std::min(static_cast< size_t >(MAX_SPELL_MANA), value), size_t(0));
   }
   inline void set_hand(HandType hand, Player player)
   {
      m_hand[player] = std::move(hand);
   }
   inline void set_deck(DeckContainer deck, Player player)
   {
      m_deck_cont[player] = std::move(deck);
   }

   inline void set_flag_can_attack(bool value, Player player)
   {
      m_can_attack[player] = value;
   }
   inline void set_flag_can_plunder(bool value, Player player)
   {
      m_can_plunder[player] = value;
   }
   inline void set_graveyard(
      std::map< size_t, std::vector< sptr< Unit > > > value, Player player)
   {
      m_graveyard[player] = std::move(value);
   }
   inline void set_tossed_card(std::vector< sptr< Card > > value, Player player)
   {
      m_tossed_cards[player] = std::move(value);
   }

   inline void set_board(sptr< Board > value) { m_board = std::move(value); }
   inline void set_history(
      std::map< size_t, std::vector< sptr< AnyAction > > > history)
   {
      m_history = std::move(history);
   }

   inline void set_starting_player(Player player)
   {
      m_starting_player = player;
      set_flag_can_attack(true, player);
      set_turn(player);
   }

   inline void set_turn(Player player) { m_turn = player; }

   inline void set_scout_token(Player player, bool value)
   {
      m_scout_token.at(player) = value;
   }

   inline void set_attacker(Player player) { m_attacker = player; }
   inline void reset_attacker() { m_attacker.reset(); }

   inline void set_spell_stack(std::vector< sptr< Spell > > spell_stack)
   {
      m_spell_stack = std::move(spell_stack);
   }

   [[nodiscard]] inline auto get_nexus_health(Player player) const
   {
      return m_nexus_health[player];
   }
   [[nodiscard]] inline auto get_mana(Player player) const
   {
      return m_mana[player];
   }
   [[nodiscard]] inline auto get_mana_gems(Player player) const
   {
      return m_managems[player];
   }
   [[nodiscard]] inline auto get_spell_mana(Player player) const
   {
      return m_spell_mana[player];
   }
   [[nodiscard]] inline auto* get_hand(Player player) const
   {
      return &m_hand[player];
   }
   [[nodiscard]] inline auto* get_hand(Player player)
   {
      return &m_hand[player];
   }
   [[nodiscard]] inline auto* get_deck(Player player) const
   {
      return &m_deck_cont[player];
   }
   [[nodiscard]] inline auto* get_deck(Player player)
   {
      return &m_deck_cont[player];
   }
   [[nodiscard]] inline auto get_flag_can_attack(Player player) const
   {
      return m_can_attack[player];
   }
   [[nodiscard]] inline auto get_scout_token(Player player) const
   {
      return m_scout_token[player];
   }
   [[nodiscard]] inline auto is_enlightened(Player player) const
   {
      return m_managems[player] == MAX_MANA;
   }
   [[nodiscard]] inline auto get_flag_can_plunder(Player player) const
   {
      return m_can_plunder[player];
   }
   [[nodiscard]] inline auto get_graveyard(Player player) const
   {
      return m_graveyard[player];
   }
   [[nodiscard]] inline auto get_tossed_cards(Player player) const
   {
      return m_tossed_cards[player];
   }
   [[nodiscard]] inline auto get_board() const { return m_board; }
   [[nodiscard]] inline auto* get_history() const { return &m_history; }
   [[nodiscard]] inline auto get_round() const { return m_round; }
   [[nodiscard]] inline auto get_turn() const { return m_turn; }
   [[nodiscard]] inline auto get_attacker() const { return m_attacker; }
   [[nodiscard]] inline auto get_starting_player() const
   {
      return m_starting_player;
   }
   [[nodiscard]] inline auto get_pass_count() const { return m_pass_count; }
   [[nodiscard]] inline auto& get_spell_stack() { return m_spell_stack; }

   inline void incr_managems(Player player, size_t amount = 1)
   {
      set_mana_gems(
         std::min(m_managems[player] + amount, size_t(MAX_MANA)), player);
   }
   inline void decr_managems(Player player, size_t amount = 1)
   {
      set_mana_gems(std::max(m_managems[player] - amount, size_t(1)), player);
   }
   inline void fill_mana(Player player) { m_mana[player] = m_managems[player]; }
   inline void incr_turn() { m_turn = Player((m_turn + 1) % 2); }
   inline void incr_round() { m_round += 1; }

   inline void shuffle_card_into_deck(const sptr< Card >& card, Player player)
   {
      auto& deck = m_deck_cont[player].get_cards();
      std::uniform_int_distribution< size_t > dist(0, deck.size());
      auto pos = deck.begin() + dist(rng::rng_def_engine);
      deck.insert(pos, card);
   }

   /*
    * Check if the current game state is terminal
    */
   inline int is_terminal()
   {
      if(! m_terminal_checked) {
         _check_terminal();
      }
      return m_terminal;
   }

   void commit_to_history(sptr< AnyAction > action);

   sptr< Card > draw_card();

   sptr< Card > draw_card_by_idx(Player player, size_t index);

   std::vector< sptr< Card > > draw_n_cards(
      Player player, size_t n, bool random = true);

   void play_unit(const sptr< Unit >& unit);

   void play_spell(const sptr< Spell >& spell);

   void move_to_graveyard(sptr< Unit > unit);
   void move_to_tossed(sptr< Card > card);

   inline void incr_pass_count() { m_pass_count += 1; }
   inline void reset_pass_count() { m_pass_count = 0; }

   void shuffle_deck(Player player);
};

#endif  // LORAINE_STATE_H
