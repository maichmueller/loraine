
#include "loraine/core/action.hpp"

#include "loraine/core/gamestate.h"
#include "loraine/core/logic.h"
#include "loraine/utils/algorithms.h"

using namespace input;


ActionFollowup PlaceSpellAction::execute_impl(GameState& state)
{
   auto reset_targets = [](std::vector< sptr< IEffect > >& effects) {
      for(auto& effect : effects) {
         effect->reset_targets();
      }
   };
   auto& hand = state.player(team()).hand();
   auto spell = to_spell(hand.at(hand_index));
   auto& effects_to_cast = spell->add_effects(events::EventLabel::CAST);
   auto& spell_stack = state.spell_stack();
   auto& s_buffer = state.buffer().spell;
   if(to_stack) {
      // add the spell to the stack and the buffer, this would allow the
      // opponent to also see that a spell might be played
      s_buffer.emplace_back(spell);
      spell_stack.emplace_back(spell);
      spell->move_to(Zone::SPELLSTACK, spell_stack.size() - 1);
      // check the effect targets of this spell
      for(auto& effect : effects_to_cast) {
         if(auto& targeter = *effect->targeter(); bool(targeter)) {
            if(targeter.is_automatic()) {
               // an automatic targeter does targeting via filtering out the cards that do not
               // meet the criteria
               targeter(state, team());
            } else {
               state.buffer().targeting.emplace_back(effect);
            }
         }
      }

      if(state.buffer().targeting.empty()) {
         // we remove the spell from the hand only once the targeting is cleared. Since in this case
         // there is no manual targeting required, we can immediately delete it from hand. Otherwise
         // the TargetAction is going to take care of this
         hand.erase(std::find(hand.begin(), hand.end(), spell));
         // a burst or focus spell is played immediately if no targeting is required
         if(spell->has_any_of({Keyword::BURST, Keyword::FOCUS})) {
            state.buffer().action.emplace_back(
               std::make_shared< Action >(PlaySpellFinishAction(team(), true)));
         }
      }
   } else {
      reset_targets(effects_to_cast);
      s_buffer.erase(
         std::remove(s_buffer.begin(), s_buffer.end(), spell), s_buffer.end());
      spell_stack.erase(
         std::remove(spell_stack.begin(), spell_stack.end(), spell), spell_stack.end());
      hand.emplace_back(spell);
      spell->move_to(Zone::HAND, hand.size() - 1);
   }
   return false;
}

ActionFollowup AdvanceUnitAction::execute_impl(GameState& state)
{
   auto& bf = state.board().battlefield(team());
   auto& camp = state.board().camp(team());
   if(to_bf) {
      for(auto idx : indices) {
         const auto& field_card = camp[idx];
         bf.emplace_back(to_unit(field_card));
         state.buffer().bf.emplace_back(to_unit(field_card));
         field_card->move_to(Zone::BATTLEFIELD, bf.size() - 1);
      }
      // removing the units from the camp from the end of the vector. This method should be fast
      // enough for small vector sizes
      algo::remove_by_indices(camp, indices);
   } else {
      for(auto idx : indices) {
         auto unit = bf[idx];
         camp.emplace_back(bf[idx]);
         auto& bf_buffer = state.buffer().bf;
         bf_buffer.erase(std::find(bf_buffer.begin(), bf_buffer.end(), unit));
         unit->move_to(Zone::CAMP, camp.size() - 1);
      }
      // removing the units from the camp from the end of the vector. This method should be fast
      // enough for small vector sizes
      algo::remove_by_indices(bf, indices);
   }
   return false;
}
ActionFollowup DragEnemyAction::execute_impl(GameState& state)
{
   auto& bf = state.board().battlefield(opponent(team()));
   auto& camp = state.board().camp(opponent(team()));
   if(to_bf) {
      auto dragged = camp[from];
      bf[to] = to_unit(dragged);
      dragged->move_to(Zone::BATTLEFIELD, to);
      // removing the units from the camp from the end of the vector.
      camp.erase(std::next(camp.begin(), from));
   } else {
      auto dragged = bf[from];
      camp.emplace_back(dragged);
      dragged->move_to(Zone::CAMP, camp.size() - 1);
      // removing the units from the camp from the end of the vector.
      bf.erase(std::next(bf.begin(), from));
   }
   return false;
}
ActionFollowup MulliganAction::execute_impl(GameState& state)
{
   Team active_team = state.active_team();
   auto& hand = state.player(active_team).hand();
   auto& deck = state.player(active_team).deck();
   // return those cards for replacement back into the deck
   for(auto i = 0; i < replace.size(); ++i) {
      if(replace[i]) {
         deck.shuffle_into(hand[i], state.rng(), 0);
      }
   }
   // replace the marked cards with newly drawn cards
   random::shuffle_inplace(deck, state.rng());
   for(auto i = 0; i < replace.size(); ++i) {
      if(replace[i]) {
         hand[i] = deck.pop();
      }
   }
   return true;
}
ActionFollowup CancelAction::execute_impl(GameState& state)
{
   auto reset_targets = [](std::vector< sptr< IEffect > >& effects) {
      for(auto& effect : effects) {
         effect->reset_targets();
      }
   };
   // the scenario of having a field spell in the play_event_triggers buffer and a spell in the
   // spell buffer is logically not possible by the rules of the game. This would imply an
   // implementation error
   auto& p_buffer = state.buffer().play;
   auto& s_buffer = state.buffer().spell;
   if(not s_buffer.empty() && utils::has_value(*p_buffer)) {
      throw std::logic_error(
         "Both buffers for spells and fieldcards hold values. This should not occur.");
   }

   if(utils::has_value(*p_buffer)) {
      // the player cancelled playing this field spell so undo all targeting for its get
      reset_targets(p_buffer.value()->add_effects(events::EventLabel::PLAY));
      p_buffer->reset();
   } else if(not s_buffer.empty()) {
      // a spell to play with targeting was cancelled so cancel its targets
      reset_targets(s_buffer.back()->add_effects(events::EventLabel::CAST));
      s_buffer.pop_back();
   }

   return false;  // initiative stays with current player
}
ActionFollowup TargetingAction::execute_impl(GameState& state)
{
   // only set the chosen targets to the last element on the targeting t_buffer, which is
   // the one that requested targets
   auto& t_buffer = state.buffer().targeting;
   t_buffer.back()->targets(targets);
   auto assoc_card = t_buffer.back()->associated_card();
   if(t_buffer.size() == 1) {  // effect to choose targets for is last in buffer
      if(assoc_card->is_spell() &&  // effect belongs to a spell
         assoc_card->has_any_of({Keyword::BURST, Keyword::FOCUS})) {
         // if the effect to target is the last one in the t_buffer, and the get belong
         // to a BURST or FOCUS spell, then a placing with subsequent targeting also triggers
         // playing it
         state.buffer().action.emplace_back(
            std::make_shared< Action >(PlaySpellFinishAction(team(), true)));
      } else if(assoc_card->is_fieldcard()) {
         if(not state.buffer().action.back()->is_play_finish()) {
            // if no replace action has occured before, then we have to place a PlayFinishAction
            state.buffer().action.emplace_back(std::make_shared< Action >(
               PlayFieldCardFinishAction(team(), state.board().camp(team()).size())));
         }
      }
   }
   // remove the last effect from the targeting t_buffer, since we just chose its targets
   t_buffer.pop_back();
   if(t_buffer.empty()) {
      state.logic()->restore_previous_invoker();
   }
   auto& hand = state.player(team()).hand();
   hand.erase(std::find(hand.begin(), hand.end(), assoc_card));
   return false;
}

ActionFollowup ButtonPressAction::execute_impl(GameState& state)
{
   // check if units have been placed on the battlefield and thus an attack or block was
   // commenced
   if(auto& bf_buffer = state.buffer().bf; not bf_buffer.empty()) {
      bf_buffer.clear();
      auto& attack_token = state.player(team()).flags().attack_token;
      if(attack_token && not state.logic()->in_combat()) {
         // if the game is not already in combat mode and the player holds an attack token, then
         // having units in the bf_buffer must mean, that the player has just declared an attack
         state.logic()->init_attack(team());
      } else if(not attack_token) {
         // reaching this point should mean that there is an active combat and the current player
         // is not the attacker, thus they moved units because they were asked to block. We are
         // ASSUMING here that the is_valid method correctly filters out actions of moving units
         // at all other times other than attack or block declarations
         state.logic()->init_block(team());
      } else {
         throw std::logic_error(
            "Player " + std::to_string(team())
            + "moved units to the battlefield but neither"
            " held the attack-token nor is engaged in combat.");
      }
   } else if(not state.player(team()).flags().has_played) {
      if(state.is_resolved()) {
         // if the entire board is resolved, then the player has simply passed initiative
         state.logic()->pass();
      } else {
         // no attack or block were declared, so whatever the current board state is, has been
         // accepted. Thus we go on to resolve the spell stack and any open combat
         state.logic()->resolve();
      }
   }
   return true;
}
ActionFollowup PlayFieldcardAction::execute_impl(GameState& state)
{
   return true;
}

ActionFollowup PlayRequestAction::execute_impl(GameState& state)
{
   auto field_card = to_fieldcard(state.player(team()).hand().at(hand_index));
   state.buffer().play.emplace(field_card);
   auto& camp = state.board().camp(team());
   if(camp.size() == state.board().max_size_camp()) {
      // we need to choose the unit we want to replace, since the camp is full
      state.logic()->transition< ReplacingActionPhase >();
      return false;
   }
   state.buffer().action.emplace_back(
      std::make_shared< Action >(PlayFieldCardFinishAction(team(), camp.size())));

   if(field_card->has_effect(events::EventLabel::PLAY)) {
      for(const auto& effect : field_card->add_effects(events::EventLabel::PLAY)) {
         if(auto& targeter = *effect->targeter(); targeter.is_automatic()) {
            targeter(state, team());
         } else {
            state.buffer().targeting.emplace_back(effect);
         }
      }
      if(not state.buffer().targeting.empty()) {
         // set the next invoker to be a target mode invoker so that targets are chosen for the
         // get in the buffer
         state.logic()->transition< TargetActionPhase >();
         return false;
      }
   }
   return false;
}
ActionFollowup PlaySpellFinishAction::execute_impl(GameState& state)
{
   auto& s_buffer = state.buffer().spell;
   // remove the spell to play from the spell buffer stack
   state.player(team()).flags().has_played = true;
   while(not s_buffer.empty()) {
      auto spell = s_buffer.back();
      s_buffer.pop_back();

      spell->uncover();
      state.logic()->spend_mana(spell);
      state.logic()->play_event_triggers(spell);
      if(m_burst) {
         // if the topmost spell is BURST spell then only that one is meant to be played
         // immediately and we exit this PlayCommand
         state.logic()->cast(true);
         return false;
      }
   }
   // we pass initiative if we played the entire stack
   return true;
}
ActionFollowup PlayFieldCardFinishAction::execute_impl(GameState& state)
{
   auto field_card = state.buffer().play.value();
   state.buffer().play->reset();
   state.player(team()).flags().has_played = true;

   field_card->uncover();
   state.logic()->spend_mana(field_card);
   state.logic()->summon(field_card, m_camp_index);
   state.logic()->play_event_triggers(field_card);
   return true;
}

ActionFollowup ChoiceAction::execute_impl(GameState& state)
{
   // put choice at first place of the vector buffer
   auto& buffer = state.buffer().choice;
   buffer.begin()->swap(*std::next(buffer.begin(), choice));
   buffer.erase(std::next(buffer.begin(), 1), buffer.end());
   return false;
}
