
#ifndef LORAINE_EVENT_LISTENER_H
#define LORAINE_EVENT_LISTENER_H

#include <map>
#include <random>
#include <vector>

#include "cards/card.h"
#include "event.h"
#include "types.h"

// forward-declare state
class State;

class EventListener {
   std::map< events::EventType, std::map< UUID, sptr< Card > > > listeners;


  public:
   void on_event(State & state, const events::VariantEvent& e);
   void register_card(const sptr< Card >& card);
   void unregister_card(const sptr< Card >& card);
};

#endif  // LORAINE_EVENT_LISTENER_H
