
#include "event.h"
#include "event_listener.h"

namespace events {

struct active_event {
   static void set(AnyEvent&& event)
   {
      m_active_event = std::move(event);

      events::event_listener.on_event(m_active_event);
   }
   static const VariantEvent& get() { return m_active_event; }

  private:
   static VariantEvent m_active_event;
};

inline void trigger(AnyEvent&& event) {
   events::active_event::set(std::move(event));
}
}  // namespace events
