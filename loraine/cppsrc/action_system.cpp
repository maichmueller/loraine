
#include "loraine/core/systems/action_system.h"

#include "loraine/core/gamestate.h"
#include "loraine/core/logic.h"
#include "loraine/core/systems/effect_system.hpp"

void ActionSystem::restore_previous_phase()
{
   if(m_prev_phase != nullptr) {
      m_action_phase = std::move(m_prev_phase);
      m_prev_phase = nullptr;
   }
}

actions::Action ActionPhaseBase::request_action(const GameState& state) const
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
         if(auto pass_act = actions::Action(actions::AcceptAction(state.active_team()));
            is_valid(pass_act)) {
            return pass_act;
         }
         return actions::Action(actions::CancelAction(state.active_team()));
      }
   }
}
actions::Action TargetActionPhase::request_action(const GameState& state) const
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
         return actions::Action(actions::CancelAction(state.active_team()));
      }
   }
}
bool TargetActionPhase::is_valid(const actions::Action& action) const
{
   return false;
}
std::vector< actions::Action > TargetActionPhase::valid_actions(const GameState& action) const
{
   return std::vector< actions::Action >();
}
bool DefaultActionPhase::is_valid(const actions::Action& action) const
{
   return false;
}
std::vector< actions::Action > DefaultActionPhase::valid_actions(const GameState& action) const
{
   return std::vector< actions::Action >();
}
bool CombatActionPhase::is_valid(const actions::Action& action) const
{
   return false;
}
std::vector< actions::Action > CombatActionPhase::valid_actions(const GameState& action) const
{
   return std::vector< actions::Action >();
}
actions::Action ReplacingActionPhase::request_action(const GameState& state) const
{
   return ActionPhaseBase::request_action(state);
}
bool ReplacingActionPhase::is_valid(const actions::Action& action) const
{
   return false;
}
std::vector< actions::Action > ReplacingActionPhase::valid_actions(const GameState& action) const
{
   return std::vector< actions::Action >();
}
bool MulliganActionPhase::is_valid(const actions::Action& action) const
{
   return false;
}
std::vector< actions::Action > MulliganActionPhase::valid_actions(const GameState& action) const
{
   return std::vector< actions::Action >();
}
