
#include "targeter.h"

#include "agent.h"
#include "state.h"

std::vector< Target > EnemyCampManTargeter::_target(
   const State& state, Player acting_player)
{
   const auto& board = state.board();
   auto& camp = board->get_camp(Player(1 - acting_player));
   std::vector< sptr< Card > > camp_copy;
    std::vector< Target > targets;
    auto&& filter = get_filter();
    for(const auto& card : camp) {
       auto target = Target(card);
       if(filter(target)) {
          targets.emplace_back(target);
       }
    }
   return targets;
}
