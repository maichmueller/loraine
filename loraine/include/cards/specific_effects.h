#ifndef LORAINE_SPECIFIC_EFFECTS_H
#define LORAINE_SPECIFIC_EFFECTS_H

#include "card.h"
#include "effect.h"
#include "event/event.h"
#include "game.h"
#include "rulesets.h"

void create_card(Game& game, Player player, SID card_id);
void create_exact_copy(Game& game, Player player, const sptr<Card>& card);

namespace cardEffectFunc {

void fading_memories_none(
   Game& game,
   const events::NoneEvent& event,
   EffectContainer& source_container);

void fading_memories_none(
   Game& game,
   const events::NoneEvent& event,
   EffectContainer& source_container);

}
#endif  // LORAINE_SPECIFIC_EFFECTS_H
