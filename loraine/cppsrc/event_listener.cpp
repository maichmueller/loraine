
#include "events/event_listener.h"

namespace events {
void EventListener::unsubscribe(Card& card)
{
   for(auto& listener : m_listener) {
      auto end = listener.end();
      auto pos_it = std::find(listener.begin(), end, &card);
      if(pos_it != end) {
         listener.erase(pos_it);
      }
   }
}

}  // namespace events