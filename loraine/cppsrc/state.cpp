

#include "core/state.h"

#include <algorithms.h>

#include "core/action.h"
#include "core/logic.h"
#include "events/lor_events/construction.h"

void State::commit_to_history(sptr< Action > action, Team team)
{
   m_history[team][m_round].emplace_back(std::move(action));
}

void State::to_graveyard(const sptr< Card >& unit)
{
   player(unit->mutables().owner).graveyard()->at(m_round).emplace_back(unit);
}
void State::to_tossed(const sptr< Card >& card)
{
   player(card->mutables().owner).tossed_cards()->emplace_back(card);
}


std::array< uptr< EventBase >, events::n_events > State::_init_events()
{
   std::array< uptr< EventBase >, events::n_events > arr;
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
   SymArr< sptr< Controller > > players,
   Team starting_team,
   random::rng_type rng)
   : m_config(cfg),
     m_players(
        {Player(
           Team(0),
           Nexus(
              Team(0), cfg.START_NEXUS_HEALTH, cfg.PASSIVE_POWERS_BLUE, cfg.NEXUS_KEYWORDS_BLUE),
           decks[0],
           std::move(players[0])),
         Player(
            Team(1),
            Nexus(Team(1), cfg.START_NEXUS_HEALTH, cfg.PASSIVE_POWERS_RED, cfg.NEXUS_KEYWORDS_RED),
            decks[1],
            std::move(players[1]))}),
     m_starting_team(starting_team),
     m_board(std::make_shared< Board >(cfg.CAMP_SIZE, cfg.BATTLEFIELD_SIZE)),
     m_logic(std::make_shared< Logic>()),
     m_attacker(starting_team),
     m_events(_init_events()),
     m_turn(starting_team),
     m_spell_stack(),
     m_spell_prestack(),
     m_rng(rng)
{
}
Team State::_random_team(random::rng_type& rng)
{
   return Team(std::uniform_int_distribution(0, 1)(rng));
}

State::State(
   const Config& cfg,
   SymArr< Deck > decks,
   SymArr< sptr< Controller > > players,
   random::rng_type rng)
   : State(cfg, std::move(decks), std::move(players), _random_team(rng), rng)
{
}
