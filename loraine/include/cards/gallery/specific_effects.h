#ifndef LORAINE_SPECIFIC_EFFECTS_H
#define LORAINE_SPECIFIC_EFFECTS_H

#include "cards/card_types/all_card_types.h"
#include "cards/effect.h"
#include "events/event.h"
#include "game.h"
#include "rulesets.h"


namespace effects {

void fading_memories_cast(
   Game& game,
   const events::CastEvent& event, Effect& source_container);


}
#endif  // LORAINE_SPECIFIC_EFFECTS_H
