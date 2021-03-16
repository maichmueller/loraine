
#ifndef LORAINE_CONTROLLER_H
#define LORAINE_CONTROLLER_H

#include "core/action.h"
#include "cards/effect.h"
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

   virtual sptr< Action > choose_action(const State& state) = 0;
   virtual sptr< MulliganAction > choose_mulligan(
      State& state,
      std::vector< sptr< Card > > hand) = 0;
   virtual std::optional< std::vector< sptr< Targetable > > > choose_targets(
      const State& state,
      sptr< EffectBase > targeting_effect) = 0;
};

#endif  // LORAINE_CONTROLLER_H
