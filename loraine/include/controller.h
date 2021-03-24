
#ifndef LORAINE_CONTROLLER_H
#define LORAINE_CONTROLLER_H

#include "cards/effect.h"
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
   Controller(const Controller& action) noexcept = default;
   Controller(Controller&& action) noexcept = default;
   virtual ~Controller() = default;
   Controller& operator=(const Controller& action) noexcept = delete;
   Controller& operator=(Controller&& action) noexcept = delete;

   [[nodiscard]] auto uuid() const { return m_uuid; }

   virtual actions::Action choose_action(const State& state) = 0;
   virtual actions::Action choose_targets(
      const State& state,
      const sptr< EffectBase >& effect) = 0;
};

#endif  // LORAINE_CONTROLLER_H
