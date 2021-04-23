
#ifndef LORAINE_GAMESTATE_H
#define LORAINE_GAMESTATE_H

#include <loraine/core/schedule.h>
#include <loraine/effects/effectmap.h>
#include <loraine/grants/grantfactory.h>

#include <array>
#include <stack>
#include <utility>
#include <vector>

#include "action_invoker.h"
#include "board.h"
#include "config.h"
#include "gamedefs.h"
#include "loraine/events/event_id.h"
#include "loraine/events/event_subscriber.h"
#include "loraine/utils/random.h"
#include "loraine/utils/types.h"

class GameState {
   friend Logic;

  public:

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

   template < RegistrationTime registration_time >
   inline void connect(entt::entity entity)
   {
      _connect_impl< events::n_events - 1, registration_time >(entity);
   }
   template < RegistrationTime registration_time >
   inline void disconnect(entt::entity entity)
   {
      _disconnect_impl< events::n_events - 1, registration_time >(entity);
   }

   template < events::EventID event_id, typename... Args >
   void trigger(Args&&... args)
   {
      // create the event corresponding to the ID first
      auto event = helpers::id_to_event_t< event_id >{std::forward< Args >(args)...};
      // get the sorted subscribers to this event type
      auto view = m_registry.view< tag::subscriber< event_id > >();
      auto scheduled_entities = std::vector< entt::entity >{view.begin(), view.end()};
      // iterate over the scheduled entities, always popping the next one according to the order
      // policy of this event
      while(not scheduled_entities.empty()) {
         auto next_to_trigger = schedule::next< event_id >(m_registry, event, scheduled_entities);
         next_to_trigger.first->on_event(event);
         scheduled_entities.erase(algo::find(scheduled_entities, next_to_trigger.second));
      }
   }

   [[nodiscard]] inline auto& registry() { return m_registry; }
   [[nodiscard]] inline auto& registry() const { return m_registry; }

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
   [[nodiscard]] inline auto starting_team() const { return m_starting_team; }
   [[nodiscard]] inline auto& player(Team team) const { return m_players[team]; }
   [[nodiscard]] inline auto& spell_stack() { return m_spell_stack; }
   [[nodiscard]] inline auto& spell_stack() const { return m_spell_stack; }
   [[nodiscard]] inline auto& rng() { return m_rng; }
   [[nodiscard]] inline auto& rng() const { return m_rng; }

   Status status();
   inline bool is_resolved() const
   {
      return m_spell_stack.empty() && m_board.battlefield(Team::BLUE).empty()
             && m_board.battlefield(Team::RED).empty();
   }

  private:
   // the registry holding all the game entities
   entt::registry m_registry;

   Config m_config;
   SymArr< entt::entity > m_players;
   Team m_starting_team;
   Board m_board;
   /// the shared stack container for the played spells
   std::vector< entt::entity > m_spell_stack = {};

   std::optional< Team > m_attacker;
   size_t m_turn;
   size_t m_round = 0;
   Status m_status = Status::ONGOING;


   random::rng_type m_rng;

   template < size_t id, RegistrationTime registration_time >
   void _connect_impl(entt::entity entity);

   template < size_t id, RegistrationTime registration_time >
   void _disconnect_impl(entt::entity entity);
};

template < size_t id, RegistrationTime registration_time >
void GameState::_connect_impl(entt::entity entity)
{
   if constexpr(id > 0) {
      _connect_impl<id - 1>(entity);
   }
}

#endif  // LORAINE_GAMESTATE_H
