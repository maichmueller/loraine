
#include "event.h"

[[maybe_unused]] event::VariantEvent event::activate_event(AnyEvent e) {
   active_event = e;
   return active_event;
}
