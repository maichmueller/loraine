
#include "core/action_handler.h"

#include "cards/effect.h"
#include "core/logic.h"

bool ActionHandlerBase::_handle(const sptr< PlaceSpellAction >& action)
{
   auto spell = action->spell();
   bool to_stack = action->to_stack();

   auto* state = m_logic->state();
   auto* hand = state->player(spell->mutables().owner).hand();
   auto& effects_to_cast = spell->effects(events::EventLabel::CAST);
   auto* spell_stack = state->spell_stack();
   auto* spell_buffer = state->spell_buffer();
   if(to_stack) {
      hand->erase(std::find(hand->begin(), hand->end(), spell));
      // add the spell to the stack and the buffer (this would theoretically allow the
      // opponent to also see that a spell might be played if the active player follows
      // through)
      spell_buffer->emplace_back(spell);
      spell_stack->emplace_back(spell);
      spell->move(Location::SPELLSTACK, spell_stack->size() - 1);

      for(auto& effect : effects_to_cast) {
         if(auto& targeter = *effect->targeter(); bool(targeter)) {
            if(targeter.is_automatic()) {
               // an automatic targeter does targeting via filtering out the cards that do not
               // meet the criteria
               targeter(*state, action->team());
            } else {
               state->targeting_buffer()->emplace_back(effect);
            }
         }
      }
      if(not state->targeting_buffer()->empty()) {
         // for a manual targeter we need to ask the controller to make the decision
         m_logic->transition_action_handler(
            std::make_unique< TargetModeHandler >(m_logic, this->clone()));
         auto targeting_action = m_logic->request_action();
      }
   } else {
      for(auto& effect : effects_to_cast) {
         effect->reset_targets();
      }
      spell_buffer->erase(
         std::remove(spell_buffer->begin(), spell_buffer->end(), spell), spell_buffer->end());
      spell_stack->erase(
         std::remove(spell_stack->begin(), spell_stack->end(), spell), spell_stack->end());
      hand->emplace_back(spell);
      spell->move(Location::HAND, hand->size() - 1);
   }
   return false;
}
bool ActionHandlerBase::_handle(const sptr< PassAction >& action)
{
   logic()->reset_pass(action->team());
   return true;
}

bool DefaultModeHandler::handle(const sptr< Action >& action)
{
   auto action_label = action->action_label();
   if(action_label == ActionLabel::PASS) {
      return ActionHandlerBase::_handle(std::dynamic_pointer_cast< PassAction >(action));
   }

   switch(action_label) {
      case ActionLabel::PLAY: {
         return _handle(std::dynamic_pointer_cast< PlayAction >(action));
      }
      case ActionLabel::PLACE_UNIT: {
         return _handle(std::dynamic_pointer_cast< PlaceUnitAction >(action));
      }
      case ActionLabel::PLACE_SPELL: {
         return ActionHandlerBase::_handle(std::dynamic_pointer_cast< PlaceSpellAction >(action));
      }
      case ActionLabel::ACCEPT: {
         return _handle(std::dynamic_pointer_cast< AcceptAction >(action));
      }
      default: {
         throw std::logic_error(
            std::string("DefaultModeHandler was given an action of label number ")
            + std::to_string(int(action_label)));
      }
   }
}

bool DefaultModeHandler::_handle(const sptr< PlayAction >& action)
{
   auto card = action->card_played();
   auto* state = logic()->state();

   *(state->play_buffer()) = card;
   if(state->board()->camp(action->team()).size() == state->config().CAMP_SIZE) {
      // the camp is actually full, ask for the card that should be removed for this fieldcard
      logic()->transition_action_handler(
         std::make_unique< TargetModeHandler >(logic(), this->clone()));
   }
   for(auto& effect : card->effects(events::EventLabel::PLAY)) {
      if(auto targeter = effect->targeter(); bool(targeter) && (not targeter->is_automatic())) {
         state->targeting_buffer()->emplace_back(effect);
      }
   }
   if(not state->targeting_buffer()->empty()) {
      // for a manual targeter we need to ask the controller to make the decision

   }
}
bool DefaultModeHandler::_handle(const sptr< AcceptAction >& action)
{
   auto* state = logic()->state();
   auto team = action->team();
   if(not utils::has_value(*state->play_buffer())) {
      if(state->spell_buffer()->empty()) {
         logic()->reset_pass(action->team());
      } else {
         for(const auto& spell : *state->spell_buffer()) {
            logic()->trigger_event< events::EventLabel::PLAY >(team, spell);
         }
      }
   } else {
      auto [fieldcard, rplc_idx] = state->play_buffer()->value();

      state->board()->camp(team)->
   }
   if(state->player(team).flags()->attack_token && not utils::has_value(state->attacker())
      && not state->board()->battlefield(team).empty()) {
      // set the acting team as an attacker
      state->attacker(team);
      state->player(team).flags()->attack_token = false;

      // change the handler as we are now in combat mode
      logic()->transition_action_handler(std::make_unique< CombatModeHandler >(logic()));
   }
   return true;
}
