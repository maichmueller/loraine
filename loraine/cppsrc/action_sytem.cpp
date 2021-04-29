
#include "loraine/core/systems/action_sytem.h"

#include "loraine/core/logic.h"
#include "loraine/core/systems/effect_system.hpp"


bool ActionSystemBase::invoke(actions::Action& action)
{
   return action.execute(*m_gamestate->state());
}
actions::Action ActionSystemBase::request_action(const GameState& state) const
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
actions::Action TargetModeInvoker::request_action(const GameState& state) const
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
bool TargetModeInvoker::is_valid(const actions::Action& action) const
{
   return false;
}
std::vector< actions::Action > TargetModeInvoker::valid_actions(const GameState& action) const
{
   return std::vector< actions::Action >();
}
bool DefaultModeInvoker::is_valid(const actions::Action& action) const
{
   return false;
}
std::vector< actions::Action > DefaultModeInvoker::valid_actions(const GameState& action) const
{
   return std::vector< actions::Action >();
}
bool CombatModeInvoker::is_valid(const actions::Action& action) const
{
   return false;
}
std::vector< actions::Action > CombatModeInvoker::valid_actions(const GameState& action) const
{
   return std::vector< actions::Action >();
}
actions::Action ReplacingModeInvoker::request_action(const GameState& state) const
{
   return ActionSystemBase::request_action(state);
}
bool ReplacingModeInvoker::is_valid(const actions::Action& action) const
{
   return false;
}
std::vector< actions::Action > ReplacingModeInvoker::valid_actions(const GameState& action) const
{
   return std::vector< actions::Action >();
}
bool MulliganModeInvoker::is_valid(const actions::Action& action) const
{
   return false;
}
std::vector< actions::Action > MulliganModeInvoker::valid_actions(const GameState& action) const
{
   return std::vector< actions::Action >();
}
