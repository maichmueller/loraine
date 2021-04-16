
#ifndef LORAINE_CONTROLLER_H
#define LORAINE_CONTROLLER_H

#include "core/action.h"
#include "core/gamedefs.h"
#include "effects/effect.h"
#include "utils/types.h"

class Target;
class Card;

class GameState;

class Controller {
   Team m_team;
   const UUID m_uuid = utils::new_uuid();

  public:
   Controller(Team team) : m_team(team) {}
   Controller(const Controller& action) noexcept = default;
   Controller(Controller&& action) noexcept = default;
   virtual ~Controller() = default;
   Controller& operator=(const Controller& action) noexcept = delete;
   Controller& operator=(Controller&& action) noexcept = delete;

   [[nodiscard]] auto uuid() const { return m_uuid; }

   virtual actions::Action choose_action(const GameState& state) = 0;
   virtual actions::Action choose_targets(const GameState& state, const sptr< IEffect >& effect) = 0;
};

#endif  // LORAINE_CONTROLLER_H
