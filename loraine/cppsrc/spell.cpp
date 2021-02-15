

#include "cards/types/spell.h"
#include "game.h"

bool Spell::_check_play_condition(const State& state) const
{
   return state.mana(mutables().owner) + state.spell_mana(mutables().owner) >= mana_cost();
}
bool Spell::check_cast_condition(const State& state) const
{
   return _check_cast_condition(state);
}
bool Spell::_check_cast_condition(const State& state) const
{
   const auto& effects = effects(events::EventType::CAST);
   // if any effect of the spell is castable, then the card can be casted.
   return std::any_of(effects.begin(), effects.end(), [&](const auto& effect) {
     return effect.check_condition(state);
   });
}
