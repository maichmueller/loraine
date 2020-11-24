

#include "cards/gallery/specific_effects.h"
#include "target.h"
#include "game.h"

void effects::fading_memories_cast(
   Game& game, const events::CastEvent& /*event*/, EffectContainer& source_container)
{
   auto targets = source_container.get_targets();
   if(targets.empty()) {
      // TODO: Log this! Something has gone wrong in the logic
      throw std::logic_error("Fading Memories effect has no chosen target.");
   }
   game.create_exact_copy(
      source_container.get_owner(), Target(targets[0]).get_card().value());
   source_container.consume();
}