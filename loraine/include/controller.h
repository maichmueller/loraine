
#ifndef LORAINE_CONTROLLER_H
#define LORAINE_CONTROLLER_H

#include "core/action.h"
#include "core/gamedefs.h"
#include "utils/types.h"

class Target;
class Card;

class State;

class Controller {
   const UUID m_uuid = utils::new_uuid();

  public:
   Controller() = default;
   virtual ~Controller() = default;
   [[nodiscard]] auto uuid() const { return m_uuid; }

   virtual sptr< Action > decide_action(const State& state) = 0;
   virtual sptr< MulliganAction > decide_mulligan(
      State& state, std::vector< sptr< Card > > hand) = 0;
   virtual std::optional<std::vector< Target >> decide_targets(
      std::vector< Target > targets, long n = -1) = 0;
};

#endif  // LORAINE_CONTROLLER_H
