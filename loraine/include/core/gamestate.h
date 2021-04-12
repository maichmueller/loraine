
#ifndef LORAINE_GAMESTATE_H
#define LORAINE_GAMESTATE_H

#include <events/event_types.h>
#include <grants/grantfactory.h>

#include <array>
#include <stack>
#include <utility>
#include <vector>

#include "action_invoker.h"
#include "board.h"
#include "config.h"
#include "events/event_labels.h"
#include "events/event_subscriber.h"
#include "gamedefs.h"
#include "nexus.h"
#include "player.h"
#include "record.h"
#include "utils/random.h"
#include "utils/types.h"

class Card;


class GameState {
   friend Logic;

   struct Buffer {
      std::optional< sptr< FieldCard > > play;  // play buffer
      std::vector< sptr< Unit > > bf;  // battlefield buffer
      std::vector< sptr< Spell > > spell;  // spell stack buffer
      std::vector< sptr< IEffect > > targeting;  // targeting buffer
      std::vector< sptr< Card > > choice;  // choice buffer
      std::vector< sptr< actions::Action > > action;  // command buffer
   };

  public:
   using SpellStackType = std::vector< sptr< Spell > >;
   using HistoryType = std::map< size_t, std::vector< uptr< Record > > >;

   GameState(
      const Config& cfg,
      SymArr< Deck > decks,
      SymArr< sptr< Controller > > controllers,
      Team starting_team,
      random::rng_type rng = random::create_rng());

   GameState(
      const Config& cfg,
      SymArr< Deck > decks,
      SymArr< sptr< Controller > > controllers,
      random::rng_type rng = random::create_rng());

   GameState(const GameState& other);

   inline auto& event(events::EventLabel label)
   {
      return m_events.at(static_cast< size_t >(label));
   }
   [[nodiscard]] inline auto& event(events::EventLabel label) const
   {
      return m_events.at(static_cast< size_t >(label));
   }

   [[nodiscard]] inline auto& board() { return m_board; }
   [[nodiscard]] inline auto& board() const { return m_board; }

   [[nodiscard]] inline auto& round() { return m_round; }
   [[nodiscard]] inline auto round() const { return m_round; }

   inline auto& turn() { return m_turn; }
   [[nodiscard]] inline auto turn() const { return m_turn; }

   inline void attacker(Team team) { m_attacker = team; }
   [[nodiscard]] inline auto attacker() const { return m_attacker; }
   inline void reset_attacker() { m_attacker.reset(); }

   [[nodiscard]] inline auto active_team() const { return Team(m_turn % 2); }

   [[nodiscard]] auto& config() const { return m_config; }
   [[nodiscard]] auto logic() const { return m_logic; }
   [[nodiscard]] inline auto starting_team() const { return m_starting_team; }
   [[nodiscard]] inline auto& player(Team team) { return m_players[team]; }
   [[nodiscard]] inline auto& player(Team team) const { return m_players[team]; }
   [[nodiscard]] inline auto& spell_stack() { return m_spell_stack; }
   [[nodiscard]] inline auto& spell_stack() const { return m_spell_stack; }
   [[nodiscard]] inline auto& buffer() { return m_buffer; }
   [[nodiscard]] inline auto& buffer() const { return m_buffer; }
   [[nodiscard]] inline auto& grantfactory(Team team) { return m_grant_factory[team]; }
   [[nodiscard]] inline auto& grantfactory(Team team) const { return m_grant_factory[team]; }
   [[nodiscard]] inline auto& history() { return m_history; }
   [[nodiscard]] inline auto& history() const { return m_history; }
   [[nodiscard]] inline auto& rng() { return m_rng; }
   [[nodiscard]] inline auto& rng() const { return m_rng; }

   Status status();
   inline bool is_resolved() const
   {
      return m_spell_stack.empty() && m_board.battlefield(Team::BLUE).empty()
             && m_board.battlefield(Team::RED).empty();
   }
   void commit_to_history(uptr< Record >&& record);
   void to_graveyard(const sptr< FieldCard >& unit);
   void to_spellyard(const sptr< Spell >& unit);
   void to_tossed(const sptr< Card >& card);

  private:
   Config m_config;
   SymArr< Player > m_players;
   Team m_starting_team;
   Board m_board;
   sptr< Logic > m_logic;
   std::array< events::LOREvent, events::n_events > m_events;
   Buffer m_buffer = {};
   std::optional< Team > m_attacker;
   size_t m_turn;
   size_t m_round = 0;
   Status m_status = Status::ONGOING;

   SpellStackType m_spell_stack{};
   SymArr< GrantFactory > m_grant_factory = {};
   uptr< HistoryType > m_history = {};
   random::rng_type m_rng;
};

#endif  // LORAINE_GAMESTATE_H
