
#ifndef LORAINE_AGENT_H
#define LORAINE_AGENT_H

#include "action.h"
#include "rulesets.h"
#include "state.h"
#include "types.h"

class BaseTarget;
class Card;

class Agent {
   UUID m_uuid;

  public:
   virtual ~Agent() = default;
   [[nodiscard]] auto get_uuid() const { return m_uuid; }
   virtual sptr< AnyAction > decide_action(const State& state) = 0;
   virtual sptr< MulliganAction > decide_mulligan(
      State& state, std::vector< sptr< Card > > hand) = 0;
   virtual std::vector< size_t > decide_targets(
      std::vector< sptr< Card > > card_container, long n = -1) = 0;
};

#endif  // LORAINE_AGENT_H
