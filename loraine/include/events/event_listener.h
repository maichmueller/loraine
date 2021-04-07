
#ifndef LORAINE_EVENT_LISTENER_H
#define LORAINE_EVENT_LISTENER_H

#include <map>
#include <vector>

#include "events/lor_events/event_types.h"
#include "utils/types.h"
#include "utils/utils.h"

// derive the listener class:
// struct MyListener : EventListener<MyListener> (i.e. CRTP)
template < class Derived >
class EventListener: public utils::CRTP< EventListener, Derived > {
   /// SFINAE to check whether the Derived listener has a 'disconnect' method.

   template < class, class = void >
   struct has_disconnect_method: std::false_type {
   };

   template < class T >
   struct has_disconnect_method< T, std::void_t< decltype(std::declval< T >().disconnect()) > >:
       std::true_type {
   };

  public:
   template < typename ConcreteEventType >
   void connect(events::LOREvent& event)
   {
      event.subscribe<ConcreteEventType>(dynamic_cast<typename ConcreteEventType::SubscriberType*>(this->derived()));
      m_subscribed_events.push_back(&event);
   }

   inline void disconnect()
   {
      if constexpr(has_disconnect_method< Derived >::value) {
         this->derived()->disconnect();
      } else {
         for(auto& event : m_subscribed_events) {
            event->unsubscribe(static_cast< void* >(this));
         }
         m_subscribed_events.clear();
      }
   }

   // when the listener dies, we must notify the m_subscribed_events to remove subscription
   ~EventListener() { disconnect(); }

   [[nodiscard]] auto* subscribed_events() { return &m_subscribed_events; }
   [[nodiscard]] auto& subscribed_events() const { return &m_subscribed_events; }

  private:
   std::vector< events::LOREvent* > m_subscribed_events{};
};

#endif  // LORAINE_EVENT_LISTENER_H
