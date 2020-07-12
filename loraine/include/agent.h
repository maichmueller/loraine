
#ifndef LORAINE_AGENT_H
#define LORAINE_AGENT_H

#include "cards/card.h"
#include "rulesets.h"
#include "types.h"
#include "action.h"

class Agent {
    UUID id;

   public:
    sptr<Action> decide_action(const State & state);
};

#endif  // LORAINE_AGENT_H
