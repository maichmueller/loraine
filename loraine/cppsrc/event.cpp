
#include "event.h"

[[maybe_unused]] events::VariantEvent events::activate_event(AnyEvent e) {
   active_event = e;
   return active_event;
}
