

#include "loraine/core/gamestate.h"

#include <loraine/utils/algorithms.h>

#include "loraine/core/action.h"
#include "loraine/core/logic.h"

void GameState::commit_to_history(uptr< Record >&& record)
{
   (*m_history)[m_round].emplace_back(std::move(record));
}

void GameState::to_graveyard(const sptr< FieldCard >& unit)
{
   player(unit->mutables().team).graveyard().at(m_round).emplace_back(unit);
}
void GameState::to_spellyard(const sptr< Spell >& unit)
{
   player(unit->mutables().team).spellyard().at(m_round).emplace_back(unit);
}
void GameState::to_tossed(const sptr< Card >& card)
{
   player(card->mutables().team).tossed_cards().emplace_back(card);
}

Status GameState::status()
{
   if(not m_status.is_checked()) {
      m_logic->check_status();
   }
   return m_status;
}

GameState::GameState(
   const Config& cfg,
   SymArr< Deck > decks,
   SymArr< sptr< Controller > > controllers,
   Team starting_team,
   random::rng_type rng)
    : m_config(cfg),
      m_players(
         {Player(
             Team(0),
             Nexus(
                Team(0),
                cfg.START_NEXUS_HEALTH,
                cfg.PASSIVE_POWERS_BLUE,
                cfg.NEXUS_KEYWORDS_BLUE),
             decks[0],
             std::move(controllers[0])),
          Player(
             Team(1),
             Nexus(Team(1), cfg.START_NEXUS_HEALTH, cfg.PASSIVE_POWERS_RED, cfg.NEXUS_KEYWORDS_RED),
             decks[1],
             std::move(controllers[1]))}),
      m_starting_team(starting_team),
      m_board_system(cfg.CAMP_SIZE, cfg.BATTLEFIELD_SIZE),
      m_attacker(starting_team)
      m_turn(starting_team),
      m_spell_stack(),
      m_rng(rng)
{
   m_logic->state(*this);
}

GameState::GameState(
   const Config& cfg,
   SymArr< Deck > decks,
   SymArr< sptr< Controller > > controllers,
   random::rng_type rng)
    : GameState(
       cfg,
       std::move(decks),
       std::move(controllers),
       Team(random::bernoulli_sample(0.5, rng)),
       rng)
{
}
GameState::GameState(const GameState& other)
    : m_config(other.m_config),
      m_players(other.m_players),
      m_starting_team(other.m_starting_team),
      m_board_system(other.m_board_system),
      m_round(other.m_round)
{
   // TODO: this needs to fully reconnect all cloned event listeners with the correct events
}
void GameState::restore_previous_phase() {
   m_phase = std::move(m_prev_phase);
   m_prev_phase = nullptr;
}
