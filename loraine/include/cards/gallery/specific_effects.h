#ifndef LORAINE_SPECIFIC_EFFECTS_H
#define LORAINE_SPECIFIC_EFFECTS_H

#include "cards/card.h"
#include "cards/effect.h"
#include "event/event.h"
#include "game.h"
#include "rulesets.h"

void create_card(Game& game, Player player, size_t card_id);
void create_exact_copy(Game& game, Player player, const sptr<Card>& card);

namespace effects {

void fading_memories_cast(
   Game& game,
   const events::NoneEvent& event,
   EffectContainer& source_container);


}
#endif  // LORAINE_SPECIFIC_EFFECTS_H
