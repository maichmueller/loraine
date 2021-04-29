

#include "loraine/core/systems/board_system.h"

#include "loraine/core/components.h"
#include "loraine/utils/utils.h"

std::vector< entt::entity > BoardSystem::camp_units(Team team) const
{
   if(team == Team::BLUE) {
      return camp_units<Team::BLUE>();
   } else {
      return camp_units<Team::RED>();
   }
}



