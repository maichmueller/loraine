

#include "core/gamestate.h"

#include <utils/algorithms.h>

#include "core/action.h"
#include "core/logic.h"
#include "events/lor_events/construction.h"

void GameState::commit_to_history(uptr< Record >&& record)
{
   (*m_history)[m_round].emplace_back(std::move(record));
}

void GameState::to_graveyard(const sptr< Card >& unit)
{
   player(unit->mutables().owner).graveyard()->at(m_round).emplace_back(unit);
}
void GameState::to_tossed(const sptr< Card >& card)
{
   player(card->mutables().owner).tossed_cards()->emplace_back(card);
}

Status GameState::status()
{
   if(not m_status_checked) {
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
      m_board(cfg.CAMP_SIZE, cfg.BATTLEFIELD_SIZE),
      m_logic(std::make_shared< Logic >()),
      m_attacker(starting_team),
      m_events(events::build_event_array()),
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
      m_events(events::build_event_array()),
      m_starting_team(other.m_starting_team),
      m_board(other.m_board),
      m_logic(other.m_logic->clone()),
      m_round(other.m_round)
{
   // TODO: this needs to fully reconnect all cloned event listeners with the correct events
}
