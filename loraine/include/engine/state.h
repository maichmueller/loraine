
#ifndef LORAINE_STATE_H
#define LORAINE_STATE_H

#include <grants/grantfactory.h>

#include <array>
#include <stack>
#include <utility>
#include <vector>

#include "action_handler.h"
#include "board.h"
#include "config.h"
#include "events/event_listener.h"
#include "gamedefs.h"
#include "nexus.h"
#include "player.h"
#include "record.h"
#include "rng.h"
#include "utils/types.h"

class Card;
class Action;
class Logic;

class State {
   friend Logic;

  public:
   using SpellStackType = std::vector< sptr< Spell > >;
   using HistoryType = std::map< size_t, std::vector< uptr< Record > > >;

   State(
      const Config& cfg,
      SymArr< Deck > decks,
      SymArr< sptr< Controller > > players,
      Team starting_team,
      rng::rng_type rng = rng::create());

   auto& events() { return m_events; }
   [[nodiscard]] auto& events() const { return m_events; }
   [[nodiscard]] inline auto board() { return m_board; }
   [[nodiscard]] inline auto board() const { return m_board; }

   [[nodiscard]] inline auto* round() { return &m_round; }
   [[nodiscard]] inline auto round() const { return m_round; }

   inline void turn(size_t turn) { m_turn = turn; }
   [[nodiscard]] inline auto turn() const { return m_turn; }

   inline void attacker(Team team) { m_attacker = team; }
   [[nodiscard]] inline auto attacker() const { return m_attacker; }
   inline void reset_attacker() { m_attacker.reset(); }

   [[nodiscard]] auto& config() const { return m_config; }
   [[nodiscard]] inline auto starting_team() const { return m_starting_team; }
   [[nodiscard]] inline auto& player(Team team) { return m_players[team]; }
   [[nodiscard]] inline auto& player(Team team) const { return m_players[team]; }
   [[nodiscard]] inline auto active_team() const { return Team(m_turn % 2); }
   [[nodiscard]] inline auto* spell_stack() { return &m_spell_stack; }
   [[nodiscard]] inline auto* spell_stack() const { return &m_spell_stack; }
   [[nodiscard]] inline auto* spell_prestack() { return &m_spell_prestack; }
   [[nodiscard]] inline auto* spell_prestack() const { return &m_spell_prestack; }
   [[nodiscard]] inline auto* grantfactory(Team team) { return &m_spell_prestack[team]; }
   [[nodiscard]] inline auto* grantfactory(Team team) const { return &m_spell_prestack[team]; }
   [[nodiscard]] inline auto* history(Team team) { return &m_history[team]; }
   [[nodiscard]] inline auto* history(Team team) const { return &m_history[team]; }
   [[nodiscard]] inline auto& rng() { return m_rng; }
   [[nodiscard]] inline auto& rng() const { return m_rng; }

   Status status();
   void commit_to_history(sptr< Action > action, Team team);
   void to_graveyard(const sptr< Card >& unit);
   void to_tossed(const sptr< Card >& card);

  private:
   Config m_config;
   SymArr< Player > m_players;
   Team m_starting_team;
   sptr< Board > m_board;
   sptr< Logic > m_logic;
   std::array< uptr< EventBase >, events::n_events > m_events;
   std::optional< Team > m_attacker;
   size_t m_turn;
   size_t m_round = 0;
   Status m_status = Status::ONGOING;
   bool m_status_checked = false;
   SymArr< GrantFactory > m_grant_factory = {};
   SymArr< HistoryType > m_history = {};
   SpellStackType m_spell_stack{};
   SpellStackType m_spell_prestack{};
   rng::rng_type m_rng;

   static std::array< uptr< EventBase >, events::n_events > _init_events();
};

#endif  // LORAINE_STATE_H
