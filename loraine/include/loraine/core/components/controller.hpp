
#ifndef LORAINE_CONTROLLER_HPP
#define LORAINE_CONTROLLER_HPP

namespace input {
class Action;
}
class GameState;
class TargetSystem;

// class Controller {
//   Team m_team;
//
//  public:
//   Controller(Team team) : m_team(team) {}
//   Controller(const Controller& action) noexcept = default;
//   Controller(Controller&& action) noexcept = default;
//   virtual ~Controller() = default;
//   Controller& operator=(const Controller& action) noexcept = delete;
//   Controller& operator=(Controller&& action) noexcept = delete;
//
//   virtual input::Action choose_action(const GameState& state) = 0;
//   virtual input::Action choose_targets(const GameState& state, const TargetSystem& targeter) = 0;
//};

struct Controller {
   std::function< input::Action(const GameState&) > choose_action_func;
   std::function< input::Action(const GameState&, const TargetSystem&) > choose_targets_func;
};

#endif  // LORAINE_CONTROLLER_HPP
