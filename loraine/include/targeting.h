
#ifndef LORAINE_TARGETING_H
#define LORAINE_TARGETING_H

#include "cards/effect.h"
#include "game.h"
#include "target.h"

template < typename T >
std::vector< sptr< Card > > elems_to_card(std::vector< sptr< T > > cont)
{
   std::vector< sptr< Card > > new_cont(cont.size());
   for(const auto& elem : cont) {
      new_cont.emplace_back(elem);
   }
   return new_cont;
}

template < size_t n_targets >
void target_camp_man(const Game& game, Player trgt_player, EffectContainer& assoc_effect)
{
   auto& camp = game.get_board()->get_camp(trgt_player);
   auto indices = game.get_agent(game.get_state()->get_turn())->decide_targets(elems_to_card(camp));
   std::vector< Target > targets(n_targets);
   for(auto idx : indices) {
      targets.emplace_back(Target{false, trgt_player, Location::CAMP, idx, camp.at(idx)});
   }
   assoc_effect.set_targets(targets);
}

#endif  // LORAINE_TARGETING_H
