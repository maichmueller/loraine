
#ifndef LORAINE_EVENT_REGISTER_H
#define LORAINE_EVENT_REGISTER_H

#include <map>
#include <vector>

#include "cards/card.h"
#include "event.h"
#include "types.h"

class EventRegister {
   events::EventType event_type;
   std::map< events::EventType, std::vector< sptr< Card > > > registers;

  public:
   void on_event(const sptr< events::AnyEvent> & e);
};

#endif  // LORAINE_EVENT_REGISTER_H
