
#include "core/action_invoker.h"

#include "core/logic.h"
#include "effects/effect.h"

// namespace actions {
//
// bool ActionInvokerBase::_handle(const PlaceSpellAction& action)
//{
//   auto spell = action.spell();
//   bool to_stack = action.to_stack();
//
//   auto& state = m_logic->state();
//   auto& hand = state->player(spell->mutables().team).hand();
//   auto& effects_to_cast = spell->get(m_subscribed_events::EventLabel::CAST);
//   auto& spell_stack = state->spell_stack();
//   auto& spell_buffer = state->spell_buffer();
//   if(to_stack) {
//      hand->erase(std::find(hand->begin(), hand->end(), spell));
//      // add the spell to the stack and the buffer (this would theoretically allow the
//      // opponent to also see that a spell might be played if the active player follows
//      // through)
//      spell_buffer->emplace_back(spell);
//      spell_stack->emplace_back(spell);
//      spell->move(BoardRegion::SPELLSTACK, spell_stack->size() - 1);
//
//      for(auto& effect : effects_to_cast) {
//         if(auto& targeter = *effect->targeter(); bool(targeter)) {
//            if(targeter.is_automatic()) {
//               // an automatic targeter does targeting via filtering out the cards that do not
//               // meet the criteria
//               targeter(*state, action.team());
//            } else {
//               state->targeting_buffer()->emplace_back(effect);
//            }
//         }
//      }
//      if(not state->targeting_buffer()->empty()) {
//         // for a manual targeter we need to ask the controller to make the decision
//         m_logic->transition(
//            std::make_unique< TargetModeHandler >(m_logic, this->clone()));
//         auto targeting_action = m_logic->request_action(100);
//      }
//   } else {
//      for(auto& effect : effects_to_cast) {
//         effect->reset_targets();
//      }
//      spell_buffer->erase(
//         std::remove(spell_buffer->begin(), spell_buffer->end(), spell), spell_buffer->end());
//      spell_stack->erase(
//         std::remove(spell_stack->begin(), spell_stack->end(), spell), spell_stack->end());
//      hand->emplace_back(spell);
//      spell->move(BoardRegion::HAND, hand->size() - 1);
//   }
//   return false;
//}
// bool ActionInvokerBase::_handle(const PassAction& action)
//{
//   logic()->reset_pass(action.team());
//   return true;
//}
//
// bool DefaultModeHandler::handle(const Action& action)
//{
//   if(action.is_pass()) {
//      return ActionInvokerBase::_handle(action.detail< PassAction >());
//   }
//   if(action.is_placing_spell()) {
//      return ActionInvokerBase::_handle(action.detail< PlaceSpellAction >());
//   }
//   if(action.is_play_finish()) {
//      return _handle(action.detail< PlayAction >());
//   }
//   if(action.is_placing_unit()) {
//      return _handle(action.detail< PlaceUnitAction >());
//   }
//   if(action.is_accept()) {
//      return _handle(action.detail< AcceptAction >());
//   }
//   // if none of these cases were fulfilled, there might be an impl error
//   throw std::logic_error(
//      std::string("DefaultModeHandler was given an action of label number ")
//      + std::to_string(int(action.label())));
//}
//
// bool DefaultModeHandler::_handle(const PlayAction& action)
//{
//   auto spell = action.card_played();
//   auto& state = logic()->state();
//
//   *(state->play_buffer()) = spell;
//   if(state->board()->camp(action.team()).size() == state->config().CAMP_SIZE) {
//      // the camp is actually full, ask for the spell that should be removed for this fieldcard
//      logic()->transition(
//         std::make_unique< TargetModeHandler >(logic(), this->clone()));
//   }
//   for(auto& effect : spell->get(m_subscribed_events::EventLabel::PLAY_FIELDCARD)) {
//      if(auto targeter = effect->targeter(); bool(targeter) && (not targeter->is_automatic())) {
//         state->targeting_buffer()->emplace_back(effect);
//      }
//   }
//   if(not state->targeting_buffer()->empty()) {
//      // for a manual targeter we need to ask the controller to make the decision
//   }
//}
// bool DefaultModeHandler::_handle(const AcceptAction& action)
//{
//   auto& state = logic()->state();
//   auto team = action.team();
//   if(not utils::has_value(*state->play_buffer())) {
//      if(state->spell_buffer()->empty()) {
//         logic()->reset_pass(action.team());
//      } else {
//         for(const auto& spell : *state->spell_buffer()) {
//            logic()->trigger_event< m_subscribed_events::EventLabel::PLAY_FIELDCARD >(team, spell);
//         }
//      }
//   } else {
//      auto fieldcard = state->play_buffer()->value();
//
//      state->board()->camp(team)->
//   }
//   if(state->player(team).flags()->attack_token && not utils::has_value(state->attacker())
//      && not state->board()->battlefield(team).empty()) {
//      // set the acting team as an attacker
//      state->attacker(team);
//      state->player(team).flags()->attack_token = false;
//
//      // change the handler as we are now in combat mode
//      logic()->transition(std::make_unique< CombatModeHandler >(logic()));
//   }
//   return true;
//}
//}  // namespace actions


bool ActionInvokerBase::invoke(actions::Action& action)
{
   return action.execute(*m_logic->state());
}
actions::Action ActionInvokerBase::request_action(const GameState& state) const
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
   return ActionInvokerBase::request_action(state);
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
