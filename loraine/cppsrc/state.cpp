

#include "core/state.h"

#include <algorithms.h>

#include "core/action.h"
#include "core/logic.h"
#include "events/lor_events/construction.h"

void State::commit_to_history(uptr< Record >&& record)
{
   (*m_history)[m_round].emplace_back(std::move(record));
}

void State::to_graveyard(const sptr< Card >& unit)
{
   player(unit->mutables().owner).graveyard()->at(m_round).emplace_back(unit);
}
void State::to_tossed(const sptr< Card >& card)
{
   player(card->mutables().owner).tossed_cards()->emplace_back(card);
}

std::array< events::LOREvent, events::n_events > State::_init_events()
{
   std::array< events::LOREvent, events::n_events > arr;
   events::fill_event_array< events::n_events >(arr);
   return arr;
}
Status State::status()
{
   if(not m_status_checked) {
      m_logic->check_status();
   }
   return m_status;
}

State::State(
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
      m_board(std::make_shared< Board >(cfg.CAMP_SIZE, cfg.BATTLEFIELD_SIZE)),
      m_logic(std::make_shared< Logic >()),
      m_attacker(starting_team),
      m_events(_init_events()),
      m_turn(starting_team),
      m_spell_stack(),
      m_rng(rng)
{
}

State::State(
   const Config& cfg,
   SymArr< Deck > decks,
   SymArr< sptr< Controller > > controllers,
   random::rng_type rng)
    : State(
       cfg,
       std::move(decks),
       std::move(controllers),
       Team(random::bernoulli_sample(0.5, rng)),
       rng)
{
}
State::State(const State& other)
    : m_config(other.m_config),
      m_players({m_players[0].deck()}other.m_players),
      m_starting_team(other.m_starting_team),
      m_board(other.m_board),
      m_logic(other.m_logic)
      m_round(other.m_round)
{
}
