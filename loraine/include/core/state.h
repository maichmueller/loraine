
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
#include "nexus.h"
#include "player.h"
#include "random.h"
#include "record.h"
#include "utils/types.h"

class Card;
class Action;
class Logic;

class State {
   friend Logic;

  public:
   using SpellStackType = std::vector< sptr< Spell > >;
   using HistoryType = std::map< size_t, std::vector< uptr< Record > > >;
   template < typename T >
   using BufferType = std::vector< T >;
   using PlayBufferType = std::optional< sptr< FieldCard > >;

   State(
      const Config& cfg,
      SymArr< Deck > decks,
      SymArr< sptr< Controller > > players,
      Team starting_team,
      random::rng_type rng = random::create_rng());

   State(
      const Config& cfg,
      SymArr< Deck > decks,
      SymArr< sptr< Controller > > players,
      random::rng_type rng = random::create_rng());

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
   [[nodiscard]] auto& logic() const { return m_logic; }
   [[nodiscard]] inline auto starting_team() const { return m_starting_team; }
   [[nodiscard]] inline auto& player(Team team) { return m_players[team]; }
   [[nodiscard]] inline auto& player(Team team) const { return m_players[team]; }
   [[nodiscard]] inline auto active_team() const { return Team(m_turn % 2); }
   [[nodiscard]] inline auto* spell_stack() { return &m_spell_stack; }
   [[nodiscard]] inline auto* spell_stack() const { return &m_spell_stack; }
   [[nodiscard]] inline auto* play_buffer() { return &m_play_buffer; }
   [[nodiscard]] inline auto* play_buffer() const { return &m_play_buffer; }
   [[nodiscard]] inline auto* spell_buffer() { return &m_spell_buffer; }
   [[nodiscard]] inline auto* spell_buffer() const { return &m_spell_buffer; }
   [[nodiscard]] inline auto* targeting_buffer() { return &m_targeting_buffer; }
   [[nodiscard]] inline auto* targeting_buffer() const { return &m_targeting_buffer; }
   [[nodiscard]] inline auto* grantfactory(Team team) { return &m_grant_factory[team]; }
   [[nodiscard]] inline auto* grantfactory(Team team) const { return &m_grant_factory[team]; }
   [[nodiscard]] inline auto* history() { return &m_history; }
   [[nodiscard]] inline auto* history() const { return &m_history; }
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
   PlayBufferType m_play_buffer;
   BufferType< sptr< Spell > > m_spell_buffer;
   BufferType< sptr< EffectBase > > m_targeting_buffer;
   std::array< events::LOREvent, events::n_events > m_events;
   std::optional< Team > m_attacker;
   size_t m_turn;
   size_t m_round = 0;
   Status m_status = Status::ONGOING;
   bool m_status_checked = false;
   SymArr< GrantFactory > m_grant_factory = {};
   sptr< HistoryType > m_history = {};
   SpellStackType m_spell_stack{};
   random::rng_type m_rng;

   static std::array< events::LOREvent, events::n_events > _init_events();
};

#endif  // LORAINE_STATE_H
