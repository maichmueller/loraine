#ifndef LORAINE_SPECIFIC_EFFECTS_H
#define LORAINE_SPECIFIC_EFFECTS_H

#include "cards/card.h"
#include "cards/effect.h"
#include "event/event.h"
#include "game.h"
#include "rulesets.h"


namespace effects {

void fading_memories_cast(
   Game& game,
   const events::CastEvent& event,
   EffectContainer& source_container);


}
#endif  // LORAINE_SPECIFIC_EFFECTS_H
