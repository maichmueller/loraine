

#include "loraine/core/systems/board_system.hpp"

#include "loraine/core/components.hpp"
#include "loraine/utils/utils.h"

std::vector< entt::entity > BoardSystem::camp_units(Team team) const
{
   if(team == Team::BLUE) {
      return camp_units<Team::BLUE>();
   } else {
      return camp_units<Team::RED>();
   }
}




