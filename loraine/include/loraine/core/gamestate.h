
#ifndef LORAINE_GAMESTATE_H
#define LORAINE_GAMESTATE_H

#include <loraine/core/schedule.h>
#include <loraine/effects/effectmap.h>
#include <loraine/grants/grantfactory.h>

#include <array>
#include <stack>
#include <utility>
#include <vector>

#include "config.h"
#include "gamedefs.h"
#include "loraine/core/components.h"
#include "loraine/core/systems/board_system.h"
#include "loraine/core/systems/input_system.h"
#include "loraine/events/event_id.h"
#include "loraine/events/event_subscriber.h"
#include "loraine/utils/random.h"
#include "loraine/utils/types.h"

class GameState {
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

   [[nodiscard]] inline auto& registry() { return m_registry; }
   [[nodiscard]] inline auto& registry() const { return m_registry; }

   [[nodiscard]] inline auto& round() { return m_round; }
   [[nodiscard]] inline auto round() const { return m_round; }

   inline auto& turn() { return m_turn; }
   [[nodiscard]] inline auto turn() const { return m_turn; }


   [[nodiscard]] inline auto active_team() const { return Team(m_turn % 2); }

   [[nodiscard]] auto& config() const { return m_config; }
   [[nodiscard]] inline auto& rng() { return m_rng; }
   [[nodiscard]] inline auto& rng() const { return m_rng; }

   Status status();
   inline bool is_resolved() const
   {
      return  m_board.empty< Zone::SPELLSTACK >() && m_board.empty< Zone::BATTLEFIELD >();
   }

  private:
   // the registry holding all the game entities
   entt::registry m_registry;

   Config m_config;

   size_t m_turn = 0;
   size_t m_round = 0;
   Status m_status = Status::ONGOING;

   InputSystem m_action_system;
   BoardSystem m_board;

   random::rng_type m_rng;

   /// private method impls

   template < size_t id, RegistrationTime registration_time >
   void _connect_impl(entt::entity entity);

   template < size_t id, RegistrationTime registration_time >
   void _disconnect_impl(entt::entity entity);
};

template < size_t id, RegistrationTime registration_time >
void GameState::_connect_impl(entt::entity entity)
{
   if constexpr(id > 0) {
      if(m_registry.all_of< EffectVector< events::EventID(id) > >(entity)) {
         m_registry.emplace< tag::subscriber< events::EventID(id) > >(entity);
      }
      _connect_impl< id - 1 >(entity);
   }
}


#endif  // LORAINE_GAMESTATE_H
