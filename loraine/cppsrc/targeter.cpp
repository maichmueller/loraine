
#include "targeter.h"

#include "agent.h"
#include "state.h"

std::vector< Target > EnemyCampManTargeter::_target(
   const State& state, Agent& agent, Player acting_player)
{
   const auto& board = state.get_board();
   auto& camp = board->get_camp(Player(1 - acting_player));
   std::vector< sptr< Card > > camp_copy;
   std::copy_if(
      camp.begin(), camp.end(), std::back_inserter(camp_copy), std::move(get_filter()));

   size_t n_targets = get_nr_targets();
   auto indices = agent.decide_targets(camp_copy, static_cast< long >(n_targets));
   std::vector< Target > targets;
   targets.reserve(n_targets);

   auto camp_first = camp.begin();
   for(auto idx : indices) {
      auto [found, unit_iter] = board->find_in_camp(to_unit(camp_copy[idx]));
      targets.emplace_back(Target{
         false,
         (*unit_iter)->get_owner(),
         Location::CAMP,
         static_cast< size_t >(std::distance(camp_first, unit_iter)),
         camp.at(idx)});
   }

   return targets;
}
