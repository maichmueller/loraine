

#include "cards/gallery/specific_effects.h"

#include "game.h"
#include "target.h"

void effects::fading_memories_cast(
   Game& game, const events::CastEvent& /*event*/, Effect& source_container)
{
   auto targets = source_container.get_targets();
   if(targets.empty()) {
      // TODO: Log this! Something has gone wrong in the logic
      throw std::logic_error("Fading Memories effect has no chosen target.");
   }
   auto target = targets[0].get_card();
   throw_if_no_value(target);
   game.create_copy(source_container.get_owner(), target.value(), true);
   source_container.consume();
}