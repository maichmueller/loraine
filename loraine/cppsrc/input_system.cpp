
#include "loraine/core/systems/input_system.h"

#include "loraine/core/gamestate.h"
#include "loraine/core/logic.h"
#include "loraine/core/systems/effect_system.hpp"

InputSystem* InputSystem::restore_previous_phase()
{
   if(m_prev_handler != nullptr) {
      m_handler = std::move(m_prev_handler);
      m_prev_handler = nullptr;
   }
   return this;
}

input::Action InputHandlerBase::request_action(const GameState& state) const
{
   int n_invalid_choices = 0;
   while(true) {
      auto action = state.player(state.active_team()).controller()->choose_action(state);

      if(is_valid(action)) {
         return action;
      }
      n_invalid_choices++;
      if(n_invalid_choices > state.config().INVALID_ACTIONS_LIMIT) {
         // TODO: Add punishment reward option to agent for RL agents for choosing invalid moves too
         //  often?
         // either passing or accepting must be valid actions at all times.
         if(auto pass_act = input::Action(input::AcceptAction(state.active_team()));
            is_valid(pass_act)) {
            return pass_act;
         }
         return input::Action(input::CancelAction(state.active_team()));
      }
   }
}
input::Action TargetActionPhase::request_action(const GameState& state) const
{
   int n_invalid_choices = 0;
   while(true) {
      auto action = state.player(state.active_team())
                       .controller()
                       ->choose_targets(state, state.buffer().targeting.back());
      ;

      if(is_valid(action)) {
         return action;
      }
      n_invalid_choices++;
      if(n_invalid_choices > state.config().INVALID_ACTIONS_LIMIT) {
         // TODO: Add punishment reward option to agent for RL agents for choosing invalid moves too
         //  often?
         return input::Action(input::CancelAction(state.active_team()));
      }
   }
}
bool TargetActionPhase::is_valid(const input::Action& action) const
{
   return false;
}
std::vector< input::Action > TargetActionPhase::valid_actions(const GameState& action) const
{
   return std::vector< input::Action >();
}
bool IdleActionHandler::is_valid(const input::Action& action) const
{
   return false;
}
std::vector< input::Action > IdleActionHandler::valid_actions(const GameState& action) const
{
   return std::vector< input::Action >();
}
bool CombatActionPhase::is_valid(const input::Action& action) const
{
   return false;
}
std::vector< input::Action > CombatActionPhase::valid_actions(const GameState& action) const
{
   return std::vector< input::Action >();
}
input::Action ReplacingActionPhase::request_action(const GameState& state) const
{
   return InputHandlerBase::request_action(state);
}
bool ReplacingActionPhase::is_valid(const input::Action& action) const
{
   return false;
}
std::vector< input::Action > ReplacingActionPhase::valid_actions(const GameState& action) const
{
   return std::vector< input::Action >();
}
bool MulliganActionPhase::is_valid(const input::Action& action) const
{
   return false;
}
std::vector< input::Action > MulliganActionPhase::valid_actions(const GameState& action) const
{
   return std::vector< input::Action >();
}
