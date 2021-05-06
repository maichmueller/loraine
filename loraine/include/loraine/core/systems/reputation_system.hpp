
#ifndef LORAINE_REPUTATION_SYSTEM_HPP
#define LORAINE_REPUTATION_SYSTEM_HPP

#include "loraine/events/event.h"
#include "loraine/events/event_subscriber.h"
#include "system.hpp"

class ReputationSystem: public ILogicSystem {
   ReputationSystem() = default;

   void on_event(const events::StrikeEvent& event);
};

#endif  // LORAINE_REPUTATION_SYSTEM_HPP
