
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
class EventListener: public utils::CRTP< EventListener, Derived > {
  public:
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
