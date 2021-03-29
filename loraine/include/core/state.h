
#ifndef LORAINE_STATE_H
#define LORAINE_STATE_H

#include <events/lor_events/event_types.h>
#include <grants/grantfactory.h>

#include <array>
#include <stack>
#include <utility>
#include <vector>

#include "action_handler.h"
#include "board.h"
#include "config.h"
#include "events/event_labels.h"
#include "events/eventbase.h"
#include "gamedefs.h"
//#include "logic.h"
#include "nexus.h"
#include "player.h"
#include "record.h"
#include "utils/random.h"
#include "utils/types.h"

class Card;

class State {
   friend Logic;

  public:
   using SpellStackType = std::vector< sptr< Spell > >;
   using HistoryType = std::map< size_t, std::vector< uptr< Record > > >;

   State(
      const Config& cfg,
      SymArr< Deck > decks,
      SymArr< sptr< Controller > > controllers,
      Team starting_team,
      random::rng_type rng = random::create_rng());

   State(
      const Config& cfg,
      SymArr< Deck > decks,
      SymArr< sptr< Controller > > controllers,
      random::rng_type rng = random::create_rng());

   State(const State& other);

   auto* events() { return &m_events; }
   [[nodiscard]] auto* events() const { return &m_events; }

   [[nodiscard]] inline auto* board() { return &m_board; }
   [[nodiscard]] inline auto* board() const { return &m_board; }

   [[nodiscard]] inline auto& round() { return m_round; }
   [[nodiscard]] inline auto round() const { return m_round; }

   inline auto& turn() { return m_turn; }
   [[nodiscard]] inline auto turn() const { return m_turn; }

   inline void attacker(Team team) { m_attacker = team; }
   [[nodiscard]] inline auto attacker() const { return m_attacker; }
   inline void reset_attacker() { m_attacker.reset(); }

   inline void requires_resolution(bool value) { m_requires_resolution = value;}
   [[nodiscard]] inline auto requires_resolution() const { return m_requires_resolution; }

   [[nodiscard]] inline auto active_team() const { return Team(m_turn % 2); }

   [[nodiscard]] auto& config() const { return m_config; }
   [[nodiscard]] auto logic() const { return m_logic; }
   [[nodiscard]] inline auto starting_team() const { return m_starting_team; }
   [[nodiscard]] inline auto& player(Team team) { return m_players[team]; }
   [[nodiscard]] inline auto& player(Team team) const { return m_players[team]; }
   [[nodiscard]] inline auto* spell_stack() { return &m_spell_stack; }
   [[nodiscard]] inline auto* spell_stack() const { return &m_spell_stack; }

   [[nodiscard]] inline auto* play_buffer() { return &std::get<0>(m_buffers); }
   [[nodiscard]] inline auto* play_buffer() const { return &std::get<0>(m_buffers); }
   [[nodiscard]] inline auto* bf_buffer() { return &std::get<1>(m_buffers); }
   [[nodiscard]] inline auto* bf_buffer() const { return &std::get<1>(m_buffers); }
   [[nodiscard]] inline auto* spell_buffer() { return &std::get<2>(m_buffers); }
   [[nodiscard]] inline auto* spell_buffer() const { return &std::get<2>(m_buffers); }
   [[nodiscard]] inline auto* targeting_buffer() { return &std::get<3>(m_buffers); }
   [[nodiscard]] inline auto* targeting_buffer() const { return &std::get<3>(m_buffers); }

   [[nodiscard]] inline auto* grantfactory(Team team) { return &m_grant_factory[team]; }
   [[nodiscard]] inline auto* grantfactory(Team team) const { return &m_grant_factory[team]; }
   [[nodiscard]] inline auto history() { return &m_history; }
   [[nodiscard]] inline auto history() const { return &m_history; }
   [[nodiscard]] inline auto& rng() { return m_rng; }
   [[nodiscard]] inline auto& rng() const { return m_rng; }

   Status status();
   void commit_to_history(uptr< Record >&& record);
   void to_graveyard(const sptr< Card >& unit);
   void to_tossed(const sptr< Card >& card);

  private:
   Config m_config;
   SymArr< Player > m_players;
   Team m_starting_team;
   Board m_board;
   sptr<Logic> m_logic;
   std::array< events::LOREvent, events::n_events > m_events;

   std::tuple<
      std::optional< sptr< FieldCard > >,
      std::vector< sptr< Unit > >,
      std::vector< sptr< Spell > >,
      std::vector< sptr< EffectBase > > >
      m_buffers = {};

   std::optional< Team > m_attacker;
   size_t m_turn;
   size_t m_round = 0;
   Status m_status = Status::ONGOING;
   bool m_requires_resolution = false;
   bool m_status_checked = false;

   SpellStackType m_spell_stack{};
   SymArr< GrantFactory > m_grant_factory = {};
   uptr< HistoryType > m_history = {};
   random::rng_type m_rng;

   static std::array< events::LOREvent, events::n_events > _init_events();
};

#endif  // LORAINE_STATE_H
