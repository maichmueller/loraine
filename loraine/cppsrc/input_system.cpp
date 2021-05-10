
#include "loraine/core/systems/input_system.h"

#include "loraine/core/gamestate.h"
#include "loraine/core/logic.h"
#include "loraine/core/systems/board_system.hpp"
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
                                .choose_targets_func(state, state.get< TargetSystem >());

      if(is_valid(action, state)) {
         return action;
      }
      n_invalid_choices++;
      if(n_invalid_choices > state.config().INVALID_ACTIONS_LIMIT) {
         // TODO: punishment reward option to agent for RL agents for choosing invalid moves too
         //  often instead?
         throw std::logic_error("Too many illegal actions chosen.");
      }
   }
}
void InitiativeInputHandler::handle(input::Action& action, GameState& state)
{
   auto& registry = state.registry();
   switch(action.id()) {
      case input::ID::BUTTONPRESS: {
         if(state.is_resolved()) {
            auto acting_team = action.team();
            if(not registry.all_of< tag::has_played >(state.player(acting_team))) {
               // the player hasn't acted (e.g. not even a burst spell activation),
               // therefore simply the initative is flipped
               if(not registry.all_of< tag::pass >(state.player(opponent(acting_team)))) {
                  registry.emplace< tag::pass >(state.active_player());
               } else {
                  // TODO: Set up a proper round end call as both have passed.
                  //                  state.get<RoundEndSystem>();
               }
            }
         } else {
            // Note: there cannot be a unit on the battlefield to declare an attack with, because we
            // would have transitioned into the attack handler before already!

            // let the spell handler take care of spell related consequences of the button press
            base::handle(action.detail< input::ButtonPressAction >(), state);
         }
         change_turn(state);
      }
      case input::ID::ADVANCE_UNIT: {
         auto& action_detail = action.detail< input::AdvanceUnitAction >();
         auto& bsystem = state.get< BoardSystem >();
         for(auto idx : action_detail.indices) {
            auto entity = bsystem.at< Zone::CAMP >(idx);
            bsystem.move_to< Zone::BATTLEFIELD >(entity, bsystem.size< Zone::BATTLEFIELD >());
         }
         m_input_system->transition< AttackInputHandler >();
      }
      case input::ID::PLACE_SPELL: {
         //         auto& action_detail = action.detail< input::PlaceSpellAction >();
         //         auto& bsystem = state.get< BoardSystem >();
         //         auto spell = bsystem->at< Zone::HAND >(action_detail.hand_index);
         //         bsystem->move_to< Zone::SPELLSTACK >(spell, bsystem->size< Zone::SPELLSTACK
         //         >());
         base::handle(action.detail< input::PlaceSpellAction >(), state);
      }
      case input::ID::PLAY_FIELDCARD: {
      }

      default: {
         throw std::logic_error("The passed action was not valid for the current handler.");
      }
   }
}
