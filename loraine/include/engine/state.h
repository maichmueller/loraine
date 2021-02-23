
#ifndef LORAINE_STATE_H
#define LORAINE_STATE_H

#include <grants/grantfactory.h>

#include <array>
#include <stack>
#include <utility>
#include <vector>

#include "action_mode.h"
#include "board.h"
#include "config.h"
#include "events/event_listener.h"
#include "gamedefs.h"
#include "nexus.h"
#include "player.h"
#include "rng_machine.h"
#include "utils/types.h"

class Card;
class Action;
class Logic;

class State {
  public:

   using SpellStackType = std::vector< sptr< Spell > >;

   bool handle(const sptr< Action >& action);

   State(
      Config cfg,
      Team starting_team,
      SymArr<Player> players)
       : m_config(cfg),
         m_players(std::move(players)),
         m_starting_team(starting_team),
         m_board(std::make_shared<Board>(cfg.CAMP_SIZE, cfg.BATTLEFIELD_SIZE)),
         m_logic(std::make_unique<Logic>()),
         m_attacker(starting_team),
         m_turn(starting_team),
         m_spell_stack()
   {
   }
   State(
      Team starting_team,

      sptr< Board > board,
      std::optional< Team > attacker,
      bool battle_mode,
      size_t round,
      Team turn,
      SymArr< bool > passes,
      Status terminal,
      bool terminal_checked,
      SpellStackType spell_stack = {},
      SpellStackType spell_prestack = {});

   auto& events() { return m_events; }
   [[nodiscard]] auto& events() const { return m_events; }

   [[nodiscard]] inline auto board() const { return m_board; }

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
   [[nodiscard]] inline auto& spell_stack() { return m_spell_stack; }
   [[nodiscard]] inline auto spell_stack() const { return m_spell_stack; }
   [[nodiscard]] inline auto& spell_prestack() { return m_spell_prestack; }
   [[nodiscard]] inline auto spell_prestack() const { return m_spell_prestack; }


   /*
    * Return the status of the current game state
    */
   Status status();

   void commit_to_history(sptr< Action > action);
   void to_graveyard(const sptr< Card >& unit);
   void to_tossed(const sptr< Card >& card);

   [[nodiscard]] std::tuple< Location, long > find(const sptr< Card >& card) const;

  private:

   Config m_config;
   SymArr<Player> m_players;
   Team m_starting_team;
   SymArr< GrantFactory > m_grant_factory;
   sptr< Board > m_board;
   uptr< Logic > m_logic;
   std::array< uptr< EventBase >, events::n_events > m_events;
   std::optional< Team > m_attacker;
   size_t m_turn;
   size_t m_round = 0;
   Status m_terminal = Status::ONGOING;
   bool m_terminal_checked = false;
   SpellStackType m_spell_stack{};
   SpellStackType m_spell_prestack{};

   void _check_terminal();

   std::array< uptr< EventBase >, events::n_events > _init_events();
};

#endif  // LORAINE_STATE_H
