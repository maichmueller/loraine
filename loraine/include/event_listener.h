
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
   std::map< events::EventType, std::vector< sptr< Card > > > listeners;
   std::random_device m_rnd_dev;

  public:
   void trigger(State & state, const events::VariantEvent& e);
   void register_card(const sptr< Card >& card);
};

#endif  // LORAINE_EVENT_LISTENER_H
