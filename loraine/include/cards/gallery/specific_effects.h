#ifndef LORAINE_SPECIFIC_EFFECTS_H
#define LORAINE_SPECIFIC_EFFECTS_H

#include "cards/card.h"
#include "cards/effect.h"
#include "engine/gamedefs.h"
#include "events/event.h"
#include "gamemode.h"

namespace effects {

void fading_memories_cast(GameMode& game,
   const events::CastEvent& event, Effect& source_container);


}
#endif  // LORAINE_SPECIFIC_EFFECTS_H
