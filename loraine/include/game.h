
#ifndef LORAINE_GAME_H
#define LORAINE_GAME_H

#include <algorithms.h>

#include <optional>
#include <utility>

#include "agent.h"
#include "cards/card.h"
#include "engine/config.h"
#include "engine/rulesets.h"
#include "engine/state.h"
#include "filters/filter.h"
#include "grants/grant.h"
#include "grants/grantfactory.h"
#include "utils/types.h"

class Game {
  public:

   bool run_game();

   Game(sptr< State > state, sptr< Agent > agent_blue, sptr< Agent > agent_red)
       : m_state(std::move(state)),
         m_agents({std::move(agent_blue), std::move(agent_red)})
   {
   }

   auto state() { return m_state; }
   [[nodiscard]] auto state() const { return m_state; }
   [[nodiscard]] inline auto& agent(Player player) const { return m_agents[player]; }

  private:
   sptr< State > m_state;
   SymArr< sptr< Agent > > m_agents;
};

#endif  // LORAINE_GAME_H
