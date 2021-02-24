
#ifndef LORAINE_EVENT_LISTENER_H
#define LORAINE_EVENT_LISTENER_H

#include <map>
#include <vector>

#include "events/event.h"
#include "events/lor_events/all_events.h"
#include "utils/types.h"

class EffectBase;

// derive the listener classes:
// struct MyListener : EventListener<MyListener> (i.e. CRTP)
template < class Derived >
class EventListener: public CRTP< EventListener, Derived > {
  public:
   // map vs array: EventType is implicitly int and thus usable as index for array.
   // Yet map, unlike array, can be empty and doesn't need to allocate empty vectors
   // for every event_type, when the listener does not hold such effects.
   // The cost is the map having to allocate effects on the heap, yet effect allocation
   // should be a comparably rare event overall, after the listener initialization.
   using EffectMap = std::map< events::EventType, std::vector< sptr< EffectBase > > >;

   template < typename SpecificEventType >
   void connect(SpecificEventType& event)
   {
      event.subscribe(this->derived());
      events.push_back(&event);
   }

   // when the listener dies, we must notify the events to remove subscription
   ~EventListener()
   {
      for(auto& event : events) {
         event->unsubscribe(static_cast< void* >(this));
      }
   }

  private:
   std::vector< EventBase* > events{};
};


#endif  // LORAINE_EVENT_LISTENER_H
