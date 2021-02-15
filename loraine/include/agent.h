
#ifndef LORAINE_AGENT_H
#define LORAINE_AGENT_H

#include "action.h"
#include "rulesets.h"
#include "state.h"
#include "types.h"

class Target;
class Card;

class Agent {
   UUID m_uuid;

  public:
   virtual ~Agent() = default;
   [[nodiscard]] auto get_uuid() const { return m_uuid; }
   virtual sptr< Action > decide_action(const State& state) = 0;
   virtual sptr< MulliganAction > decide_mulligan(
      State& state, std::vector< sptr< Card > > hand) = 0;
   virtual std::optional<std::vector< Target >> decide_targets(
      std::vector< Target > targets, long n = -1) = 0;
};

#endif  // LORAINE_AGENT_H
