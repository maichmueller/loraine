
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
   using ListenerType = std::vector< sptr<Card> >;

  private:
   std::array< ListenerType, sizeof(events::EventType) > m_listeners;


  public:
   inline ListenerType& operator[](events::EventType eve_type)
   {
      return m_listeners.at(size_t(eve_type));
   }

   void on_event(Game& game, const events::VariantEvent& e);
   inline void register_card(const sptr< Card >& card);
   inline void unregister_card(const sptr< Card >& card);

   void clear_registers();
};

}  // namespace events

#endif  // LORAINE_EVENT_LISTENER_H
