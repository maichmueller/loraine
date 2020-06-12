
#include "event_listener.h"

void EventListener::register_card(const sptr< Card >& card)
{
   for(auto& [eve_type, effect] : card->get_effects()) {
      if(! effect.is_null()) {
         listeners[eve_type].emplace_back(card);
      }
   }
}