

#include "cards/types/spell.h"

#include "core/gamemode.h"

bool Spell::_check_play_condition(const State& state) const
{
   const auto* mana = state.player(mutables().owner).mana();
   return mana->common + mana->floating >= mana_cost();
}

