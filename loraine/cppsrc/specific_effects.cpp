

#include "cards/gallery/specific_effects.h"

#include "core/gamemode.h"
#include "core/targeting.h"

//void m_effects::fading_memories_cast(
//   GameMode& game, const m_subscribed_events::CastEvent& /*m_subscribed_events*/, Effect& source_container)
//{
//   auto targets = source_container.targets();
//   if(targets.empty()) {
//      // TODO: Log this! Something has gone wrong in the logic
//      throw std::logic_error("Fading Memories effect has no chosen target.");
//   }
//   auto target = targets[0].get_card();
//   throw_if_no_value(target);
//   game.create_copy(source_container.get_owner(), target.value(), true);
//   source_container.consume();
//}