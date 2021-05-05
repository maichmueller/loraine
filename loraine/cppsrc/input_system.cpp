
#include "loraine/core/systems/input_system.h"

#include "loraine/core/gamestate.h"
#include "loraine/core/logic.h"
#include "loraine/core/systems/effect_system.hpp"

input::Action InputSystem::request_action(const GameState& state) const
{
   return m_handler->request_action(state);
}

InputSystem* InputSystem::restore_handler()
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
   auto& registry = state.registry();
   while(true) {
      input::Action action = registry.get< Controller >(state.active_player())
                                .choose_action_func(state);

      if(is_valid(action, state)) {
         return action;
      }
      n_invalid_choices++;
      if(n_invalid_choices > state.config().INVALID_ACTIONS_LIMIT) {
         // TODO: Add punishment reward option to agent for RL agents for choosing invalid moves too
         //  often?
         //  Let player lose instead!
         throw std::logic_error("Too many illegal actions chosen.");
      }
   }
}
void InputHandlerBase::change_turn(GameState& state)
{
   state.turn()++;
}

input::Action TargetInputHandler::request_action(const GameState& state) const
{
   int n_invalid_choices = 0;
   auto& registry = state.registry();
   while(true) {
      input::Action action = registry.get< Controller >(state.active_player())
                                .choose_targets_func(state, state.target_system());
      ;

      if(is_valid(action, state)) {
         return action;
      }
      n_invalid_choices++;
      if(n_invalid_choices > state.config().INVALID_ACTIONS_LIMIT) {
         // TODO: Add punishment reward option to agent for RL agents for choosing invalid moves too
         //  often?
         throw std::logic_error("Too many illegal actions chosen.");
      }
   }
}
void InitiativeInputHandler::handle(input::Action& action, GameState& state)
{
   auto& registry = state.registry();
   switch(action.id()) {
      case input::ID::PASS: {
         registry.emplace< tag::pass >(state.player(action.team()));
         change_turn(state);
      }
      case input::ID::PLACE_UNIT: {
         auto& action_detail = action.detail< input::PlaceUnitAction >();
         if(action_detail.to_bf) {
            auto& bsystem = state.get< BoardSystem >();
            for(auto idx : action_detail.indices_vec) {
               auto entity = bsystem.at<Zone::CAMP>(idx);
               bsystem.move_to<Zone::BATTLEFIELD>(entity, idx);
            }
         }
      }
      case input::ID::PLACE_SPELL: {
      }
      case input::ID::PLAY_SPELL: {
      }
      case input::ID::PLAY_FIELDCARD: {
      }
      case input::ID::ACCEPT: {
      }
      default: {
         throw std::logic_error("The passed action was not valid for the current handler.");
      }
   }
}
