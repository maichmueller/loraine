
#ifndef LORAINE_CONTROLLER_H
#define LORAINE_CONTROLLER_H

#include "core/action.h"
#include "core/gamedefs.h"
#include "loraine/core/systems/effect_system.hpp"
#include "utils/types.h"

class Target;
class Card;

class GameState;
class TargetSystem;

class Controller {
   Team m_team;

  public:
   Controller(Team team) : m_team(team) {}
   Controller(const Controller& action) noexcept = default;
   Controller(Controller&& action) noexcept = default;
   virtual ~Controller() = default;
   Controller& operator=(const Controller& action) noexcept = delete;
   Controller& operator=(Controller&& action) noexcept = delete;

   virtual input::Action choose_action(const GameState& state) = 0;
   virtual input::Action choose_targets(const GameState& state, const TargetSystem& targeter) = 0;
};

#endif  // LORAINE_CONTROLLER_H
