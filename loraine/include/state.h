
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
   using HandType = std::array< sptr< Card >, HAND_CARDS_LIMIT >;

   // player symmetric attributes

   SymArr< SID > m_nexus_ids{0, 1};
   SymArr< int > m_nexus_health{START_NEXUS_HEALTH, START_NEXUS_HEALTH};
   SymArr< size_t > m_mana{0, 0};
   SymArr< size_t > m_mana_gems{0, 0};
   SymArr< size_t > m_spell_mana{0, 0};

   SymArr< bool > m_can_attack{false, false};
   SymArr< bool > m_is_enlightened{false, false};
   SymArr< bool > m_can_plunder{false, false};

   // the number of dead allies per player
   SymArr< size_t > m_dead_ally_count{0, 0};
   // the number of dead allies per player this round
   SymArr< size_t > m_dead_ally_count_rnd{0, 0};

   // the cards in hand (short: the player's 'hand')
   SymArr< HandType > m_hand;
   SymArr< DeckContainer > m_deck_cont;

   SymArr< std::vector< sptr< Card > > > m_graveyard{};
   SymArr< std::vector< sptr< Card > > > m_tossed_cards{};
   sptr< Board > m_board = std::make_shared< Board >();

   std::vector< sptr< Action > > m_history;

   // state attributes
   PLAYER m_starting_player;
   size_t m_round = 0;
   PLAYER m_turn;
   std::optional< PLAYER > m_attacker;
   int m_terminal;
   bool m_terminal_checked;
   std::vector< sptr< Spell > > m_spell_stack;

   inline void _commit_to_history(sptr< Action > action);

   void _check_terminal();

   void _check_enlightenment() const;

  public:
   State();

   inline void set_nexus_health(int value, PLAYER player)
   {
      m_nexus_health[player] = value;
   }
   inline void set_mana(size_t value, PLAYER player) { m_mana[player] = value; }

   inline void set_mana_gems(size_t value, PLAYER player)
   {
      m_mana_gems[player] = value;
      _check_enlightenment();
   }
   inline void set_spell_mana(size_t value, PLAYER player)
   {
      m_spell_mana[player] = value;
   }
   inline void set_hand(HandType hand, PLAYER player)
   {
      m_hand[player] = std::move(hand);
   }
   inline void set_deck(DeckContainer deck, PLAYER player)
   {
      m_deck_cont[player] = std::move(deck);
   }

   inline void set_flag_can_attack(bool value, PLAYER player)
   {
      m_can_attack[player] = value;
   }
   inline void set_flag_is_enlightened(bool value, PLAYER player)
   {
      m_is_enlightened[player] = value;
   }
   inline void set_flag_can_plunder(bool value, PLAYER player)
   {
      m_can_plunder[player] = value;
   }

   inline void set_dead_ally_count(size_t value, PLAYER player)
   {
      m_dead_ally_count[player] = value;
   }
   inline void set_dead_ally_count_rnd(size_t value, PLAYER player)
   {
      m_dead_ally_count_rnd[player] = value;
   }
   inline void set_graveyard(std::vector< sptr< Card > > value, PLAYER player)
   {
      m_graveyard[player] = std::move(value);
   }
   inline void set_tossed_card(std::vector< sptr< Card > > value, PLAYER player)
   {
      m_tossed_cards[player] = std::move(value);
   }

   inline void set_board(sptr< Board > value) { m_board = std::move(value); }
   inline void set_history(std::vector< sptr< Action > > history)
   {
      m_history = std::move(history);
   }

   inline void set_starting_player(PLAYER player)
   {
      m_starting_player = player;
      set_flag_can_attack(true, player);
      set_turn(player);
   }

   inline void set_turn(PLAYER player) { m_turn = player; }

   inline void set_attacker(PLAYER player) { m_attacker = player; }
   inline void reset_attacker() { m_attacker.reset(); }

   inline void set_spell_stack(std::vector< sptr< Spell > > spell_stack)
   {
      m_spell_stack = std::move(spell_stack);
   }

   [[nodiscard]] inline auto get_nexus_health(PLAYER player) const
   {
      return m_nexus_health[player];
   }
   [[nodiscard]] inline auto get_mana(PLAYER player) const
   {
      return m_mana[player];
   }
   [[nodiscard]] inline auto get_mana_gems(PLAYER player) const
   {
      return m_mana_gems[player];
   }
   [[nodiscard]] inline auto get_spell_mana(PLAYER player) const
   {
      return m_spell_mana[player];
   }
   [[nodiscard]] inline auto* get_hand(PLAYER player) const
   {
      return &m_hand[player];
   }
   [[nodiscard]] inline auto* get_deck(PLAYER player) const
   {
      return &m_deck_cont[player];
   }
   [[nodiscard]] inline auto get_flag_can_attack(PLAYER player) const
   {
      return m_can_attack[player];
   }
   [[nodiscard]] inline auto get_flag_is_enlightened(PLAYER player) const
   {
      return m_is_enlightened[player];
   }
   [[nodiscard]] inline auto get_flag_can_plunder(PLAYER player) const
   {
      return m_can_plunder[player];
   }
   [[nodiscard]] inline auto get_dead_ally_count(PLAYER player) const
   {
      return m_dead_ally_count[player];
   }
   [[nodiscard]] inline auto get_dead_ally_count_rnd(PLAYER player) const
   {
      return m_dead_ally_count_rnd[player];
   }
   [[nodiscard]] inline auto get_graveyard(PLAYER player) const
   {
      return m_graveyard[player];
   }
   [[nodiscard]] inline auto get_tossed_cards(PLAYER player) const
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
   [[nodiscard]] inline auto& get_spell_stack() { return m_spell_stack; }

   inline void incr_managems(size_t amount, PLAYER player)
   {
      set_mana_gems(
         std::min(m_mana_gems[player] + amount, size_t(MAX_MANA)), player);
   }
   inline void incr_turn() { m_turn = PLAYER((m_turn + 1) % 2); }

   inline void shuffle_card_into_deck(const sptr< Card >& card, PLAYER player)
   {
      auto& deck = m_deck_cont[player].get_cards();
      std::uniform_int_distribution< size_t > dist(0, deck.size());
      auto pos = deck.begin() + dist(rng::rng_def_engine);
      deck.insert(pos, card);
   }

   /*
    * Perform the passed action
    */
   void do_action(const sptr< Action >& action);

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

   sptr< Card > draw_card();

   void play_unit(const sptr< Card >& unit);

   void play_spell(const sptr< Card >& spell);

   void move_to_graveyard(sptr< Card > card, PLAYER player);
   void move_to_tossed(sptr< Card > card, PLAYER player);
};

#endif  // LORAINE_STATE_H
