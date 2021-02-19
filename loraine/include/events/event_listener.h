
#ifndef LORAINE_EVENT_LISTENER_H
#define LORAINE_EVENT_LISTENER_H

#include <vector>

#include "events/event.h"
#include "utils/types.h"

// derive the listener classes:
// struct MyListener : EventListener<MyListener> (i.e. CRTP)
template < class Derived >
class EventListener {
  private:
   std::vector< EventBase* > events{};

  public:
   template < events::EventType e_type, class Context, class... Args >
   void connect(Event< Context, e_type, Args... >& event)
   {
      event.subscribe(static_cast< Derived* >(this));
      events.push_back(&event);
   }

   // when the listener dies, we must notify the events to remove subscription
   ~EventListener()
   {
      for(auto& event : events) {
         event->unsubscribe(static_cast< void* >(this));
      }
   }
};

#endif  // LORAINE_EVENT_LISTENER_H
