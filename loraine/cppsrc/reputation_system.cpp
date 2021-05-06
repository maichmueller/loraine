
#include "loraine/core/systems/reputation_system.hpp"

#include "loraine/core/components.hpp"
#include "loraine/core/gamestate.h"

void ReputationSystem::on_event(const events::StrikeEvent& event)
{
   auto [gstate, team, striker, struck] = event.data();
   auto& power = m_registry.get< PowerStat >(striker);
   if(power.power_cache > 4) {
      auto player_entt = gstate.player(team);
      auto& rep = m_registry.get< Reputation >(player_entt);
      rep.counter += 1;
      if(rep.counter >= gstate.config().REPUTATION_COUNT) {
         m_registry.emplace< tag::reputation >(player_entt);
         // inform the game participants that the player has gained reputation
         gstate.get<EffectSystem>().trigger< events::EventID::REPUTATION >(team, striker);
      }
   }
}