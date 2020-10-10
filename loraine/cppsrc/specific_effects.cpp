

#include "cards/specific_effects.h"

void cardEffectFunc::fading_memories_none(
   Game& game,
   const events::NoneEvent& /*event*/,
   EffectContainer& source_container)
{
   auto targets = source_container.get_targets();
   if(! targets.has_value()) {
      // TODO: Log this! Something has gone wrong in the logic
      throw std::logic_error("Fading Memories effect has no chosen target.");
   }
   create_exact_copy(
      game, source_container.get_owner(), targets.value()[0].card);
}