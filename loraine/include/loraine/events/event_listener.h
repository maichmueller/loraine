
#ifndef LORAINE_EVENT_LISTENER_H
#define LORAINE_EVENT_LISTENER_H

#include <map>
#include <vector>

#include "event_types.h"
#include "loraine/utils/types.h"
#include "loraine/utils/utils.h"

//// derive the listener class:
//// struct MyListener : EventListener<MyListener> (i.e. CRTP)
//template < class Derived >
//class EventListener: public utils::CRTP< EventListener, Derived > {
//  public:
//   void connect(events::LOREvent& event)
//   {
//      event.subscribe(this->derived());
//      m_subscribed_events.push_back(&event);
//   }
//
//   inline void disconnect()
//   {
//      for(auto& event : m_subscribed_events) {
//         event->unsubscribe(this->derived());
//      }
//      m_subscribed_events.clear();
//   }
//
//   [[nodiscard]] auto& subscribed_events() { return m_subscribed_events; }
//   [[nodiscard]] auto& subscribed_events() const { return m_subscribed_events; }
//
//  private:
//   std::vector< events::LOREvent* > m_subscribed_events{};
//};

#endif  // LORAINE_EVENT_LISTENER_H