
#include "event_register.h"


void EventRegister::on_event(const sptr< events::AnyEvent> & e) {
   auto & curr_events = registers[e->event_type];

}