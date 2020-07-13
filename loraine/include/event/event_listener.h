
#ifndef LORAINE_EVENT_LISTENER_H
#define LORAINE_EVENT_LISTENER_H

#include <map>
#include <random>
#include <vector>

#include "cards/card.h"
#include "event/event.h"
#include "state.h"
#include "types.h"

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
   std::array< ListenerType, sizeof(events::EventType) > m_listeners;
   State* m_state_ptr;

   inline bool _state_is_set() { return m_state_ptr == nullptr; }

  public:
   inline ListenerType& operator[](events::EventType eve_type)
   {
      return m_listeners.at(size_t(eve_type));
   }

   void on_event(const events::VariantEvent& e);
   inline void register_card(const sptr< Card >& card);
   inline void unregister_card(const sptr< Card >& card);
   inline void register_state(State& state, bool clear = true);
   inline void unregister_state();

   void clear_registers();
};

// the global event listener for all
extern EventListener event_listener;

}  // namespace events

#endif  // LORAINE_EVENT_LISTENER_H
