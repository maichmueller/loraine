
#include "targeter.h"

#include "controller.h"
#include "engine/state.h"

std::vector< Target > EnemyCampManTargeter::_target(
   const State& state, Team acting_team)
{
   const auto& board = state.board();
   auto& camp = board->camp(Team(1 - acting_team));
   std::vector< sptr< Card > > camp_copy;
    std::vector< Target > targets;
    auto&& filter = filter();
    for(const auto& card : camp) {
       auto target = Target(card);
       if(filter(target)) {
          targets.emplace_back(target);
       }
    }
   return targets;
}
