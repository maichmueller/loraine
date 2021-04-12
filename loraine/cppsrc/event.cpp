
#include "events/event.h"

#include "events/event_types.h"

namespace events {

void LOREvent::trigger(EventBus& bus, GameState& state)
{
   {
      std::visit(
         [&](auto& event) {
            using EventT = std::decay_t< decltype(event) >;
            event.trigger(state);
            bus.inform(state, event);
         },
         m_event_detail);
   }
}
}  // namespace events