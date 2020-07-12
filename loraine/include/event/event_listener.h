
#ifndef LORAINE_EVENT_LISTENER_H
#define LORAINE_EVENT_LISTENER_H

#include <map>
#include <random>
#include <vector>

#include "cards/card.h"
#include "event/event.h"
#include "types.h"

// forward-declare state
class State;

namespace events {

class EventListener {
  public:
   using ListenerType = std::map< UUID, sptr< Card > >;

  private:
   /*
    * The event listeners in one container. The respective event type listener
    * is accessed via listeners[event_type].
    *
    * we can use an array instead of map<EventType, value>, because EventType is
    * an Enum and thus provides the same access capability as a map
    */
   std::array< ListenerType, sizeof(events::EventType) > listeners;

  public:
   inline ListenerType& operator[](events::EventType eve_type)
   {
      return listeners.at(size_t(eve_type));
   }

   void on_event(State& state, const events::VariantEvent& e);
   void register_card(const sptr< Card >& card);
   void unregister_card(const sptr< Card >& card);


};

// the global event listener for all
extern EventListener event_listener;
}  // namespace events
#endif  // LORAINE_EVENT_LISTENER_H
