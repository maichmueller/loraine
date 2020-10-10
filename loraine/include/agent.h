
#ifndef LORAINE_AGENT_H
#define LORAINE_AGENT_H

#include "action.h"
#include "cards/card.h"
#include "rulesets.h"
#include "types.h"

class Agent {
   UUID m_uuid;

  public:
   [[nodiscard]] auto get_uuid() const {return m_uuid;}
   virtual sptr< AnyAction > decide_action(const State& state) = 0;
   virtual sptr< MulliganAction > decide_mulligan(
      State& state, std::vector< sptr< Card > > hand) = 0;
};

#endif  // LORAINE_AGENT_H
