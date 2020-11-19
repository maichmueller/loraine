

#include "cards/gallery/specific_effects.h"
#include "target.h"

void effects::fading_memories_cast(
   Game& game, const events::NoneEvent& /*event*/, EffectContainer& source_container)
{
   auto targets = source_container.get_targets();
   if(targets.empty()) {
      // TODO: Log this! Something has gone wrong in the logic
      throw std::logic_error("Fading Memories effect has no chosen target.");
   }
   create_exact_copy(
      game,
      source_container.get_owner(),
      to_card_target(targets[0])->get_contextcard().card);
   source_container.consume();
}