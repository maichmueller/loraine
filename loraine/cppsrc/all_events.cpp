
#include "events/lor_events/all_events.h"
#include "cards/card.h"
#include "cards/effect.h"

void events::PlayEvent::trigger(State& state, Player player, Card* played_card)
{
   const auto& subs = get_subscribers();
   if(auto prio_effect = std::find_if(subs.begin(), subs.end(), [&](Subscriber* effect) {
      return *(effect->associated_card()) == *played_card;
      }); prio_effect != subs.end()) {
      _notify(*prio_effect, state, player, played_card);
   }
   Event::trigger(state, player, played_card);
}
