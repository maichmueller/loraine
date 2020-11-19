
#include "targeter.h"

#include "agent.h"
#include "state.h"

std::vector< sptr< BaseTarget > > EnemyCampManTargeter::_target(
   const State& state, Agent& agent, Player acting_player)
{
   const auto& board = state.get_board();
   auto& camp = board->get_camp(Player(1 - acting_player));
   std::vector< sptr< Card > > camp_copy;
   auto&& filter = get_filter();
   std::copy_if(
      camp.begin(), camp.end(), std::back_inserter(camp_copy), [&](const sptr< Unit >& unit) {
         return filter(std::make_shared< CardTarget >(
            unit->get_owner(), Location::CAMP, board->index_camp(unit), unit));
      });

   size_t n_targets = get_nr_targets();
   auto indices = agent.decide_targets(camp_copy, static_cast< long >(n_targets));
   std::vector< sptr< BaseTarget > > targets;
   targets.reserve(n_targets);

   auto camp_first = camp.begin();
   for(auto idx : indices) {
      auto [found, unit_iter] = board->find_in_camp(to_unit(camp_copy[idx]));
      targets.emplace_back(std::make_shared< CardTarget >(
         (*unit_iter)->get_owner(),
         Location::CAMP,
         static_cast< size_t >(std::distance(camp_first, unit_iter)),
         camp.at(idx)));
   }

   return targets;
}
