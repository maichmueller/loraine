
#ifndef LORAINE_EVENT_REGISTER_H
#define LORAINE_EVENT_REGISTER_H

#include <map>
#include <vector>

#include "card.h"
#include "event.h"
#include "types.h"

class EventRegister {
   event::EventType event_type;
   std::map< event::EventType, std::vector< sptr< Card > > > registers;

  public:
   void on_event(const sptr<event::AnyEvent> & e);
};

#endif  // LORAINE_EVENT_REGISTER_H
