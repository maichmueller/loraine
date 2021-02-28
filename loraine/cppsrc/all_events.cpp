
#include "events/lor_events/all_events.h"
#include "cards/card.h"
#include "cards/effect.h"

void events::PlayEvent::trigger(State& state, Team team, Card* played_card)
{
   const auto& subs = subscribers();

//   if(auto prio_effect = std::find_if(sub_queue.begin(), sub_queue.end(), [&](Subscriber* effect) {
//      return *(effect->associated_card()) == *played_card;
//      }); prio_effect != sub_queue.end()) {
//      _notify(*prio_effect, state, team, played_card);
//   }
//   Event::trigger(state, team, played_card);
}
