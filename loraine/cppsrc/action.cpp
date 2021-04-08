
#include "core/action.h"

#include "core/gamestate.h"
#include "core/logic.h"
#include "utils/algorithms.h"

bool actions::PlaceSpellAction::execute_impl(GameState& state)
{
   auto reset_targets = [](std::vector< sptr< EffectBase > >& effects) {
      for(auto& effect : effects) {
         effect->reset_targets();
      }
   };
   auto* hand = state.player(team()).hand();
   auto spell = to_spell(hand->at(m_hand_index));
   auto& effects_to_cast = spell->effects(events::EventLabel::CAST);
   auto* spell_stack = state.spell_stack();
   auto* spell_buffer = state.spell_buffer();
   if(m_to_stack) {
      // add the spell to the stack and the buffer, this would allow the
      // opponent to also see that a spell might be played
      spell_buffer->emplace_back(spell);
      spell_stack->emplace_back(spell);
      spell->move(Location::SPELLSTACK, spell_stack->size() - 1);
      // check the effect targets of this spell
      for(auto& effect : effects_to_cast) {
         if(auto& targeter = *effect->targeter(); bool(targeter)) {
            if(targeter.is_automatic()) {
               // an automatic targeter does targeting via filtering out the cards that do not
               // meet the criteria
               targeter(state, team());
            } else {
               state.targeting_buffer()->emplace_back(effect);
            }
         }
      }

      if(state.targeting_buffer()->empty()) {
         // we remove the spell from the hand only once the targeting is cleared. Since in this case
         // there is no manual targeting required, we can immediately delete it from hand. Otherwise
         // the TargetAction is going to take care of this
         hand->erase(std::find(hand->begin(), hand->end(), spell));
         // a burst or focus spell is played immediately if no targeting is required
         if(spell->has_any_keyword({Keyword::BURST, Keyword::FOCUS})) {
            state.action_buffer()->emplace_back(
               std::make_shared< Action >(PlaySpellFinishAction(team(), true)));
         }
      }
   } else {
      reset_targets(effects_to_cast);
      spell_buffer->erase(
         std::remove(spell_buffer->begin(), spell_buffer->end(), spell), spell_buffer->end());
      spell_stack->erase(
         std::remove(spell_stack->begin(), spell_stack->end(), spell), spell_stack->end());
      hand->emplace_back(spell);
      spell->move(Location::HAND, hand->size() - 1);
   }
   return false;
}

bool actions::PlaceUnitAction::execute_impl(GameState& state)
{
   auto& bf = *state.board()->battlefield(team());
   auto& camp = *state.board()->camp(team());
   if(m_to_bf) {
      for(auto idx : m_indices_vec) {
         const auto& field_card = camp[idx];
         bf.emplace_back(to_unit(field_card));
         state.bf_buffer()->emplace_back(to_unit(field_card));
         field_card->move(Location::BATTLEFIELD, bf.size() - 1);
      }
      // removing the units from the camp from the end of the vector. This method should be fast
      // enough for small vector sizes
      algo::remove_by_indices(camp, m_indices_vec);
   } else {
      for(auto idx : m_indices_vec) {
         auto unit = bf[idx];
         camp.emplace_back(bf[idx]);
         auto* bf_buffer = state.bf_buffer();
         bf_buffer->erase(std::find(bf_buffer->begin(), bf_buffer->end(), unit));
         unit->move(Location::CAMP, camp.size() - 1);
      }
      // removing the units from the camp from the end of the vector. This method should be fast
      // enough for small vector sizes
      algo::remove_by_indices(bf, m_indices_vec);
   }
   return false;
}
bool actions::DragEnemyAction::execute_impl(GameState& state)
{
   auto& bf = *state.board()->battlefield(opponent(team()));
   auto& camp = *state.board()->camp(opponent(team()));
   if(m_to_bf) {
      auto dragged = camp[m_from];
      bf[m_to] = to_unit(dragged);
      dragged->move(Location::BATTLEFIELD, m_to);
      // removing the units from the camp from the end of the vector.
      camp.erase(std::next(camp.begin(), m_from));
   } else {
      auto dragged = bf[m_from];
      camp.emplace_back(dragged);
      dragged->move(Location::CAMP, camp.size() - 1);
      // removing the units from the camp from the end of the vector.
      bf.erase(std::next(bf.begin(), m_from));
   }
   return false;
}
bool actions::MulliganAction::execute_impl(GameState& state)
{
   Team active_team = state.active_team();
   auto& hand = *state.player(active_team).hand();
   auto& deck = *state.player(active_team).deck();
   // return those cards for replacement back into the deck
   for(auto i = 0; i < m_replace.size(); ++i) {
      if(m_replace[i]) {
         deck.shuffle_into(hand[i], state.rng(), 0);
      }
   }
   // replace the marked cards with newly drawn cards
   random::shuffle_inplace(deck, state.rng());
   for(auto i = 0; i < m_replace.size(); ++i) {
      if(m_replace[i]) {
         hand[i] = deck.pop();
      }
   }
   return true;
}
bool actions::CancelAction::execute_impl(GameState& state)
{
   auto reset_targets = [](std::vector< sptr< EffectBase > >& effects) {
      for(auto& effect : effects) {
         effect->reset_targets();
      }
   };
   // the scenario of having a field spell in the play_event_triggers buffer and a spell in the
   // spell buffer is logically not possible by the rules of the game. This would imply an
   // implementation error
   auto* play_buffer = state.play_buffer();
   auto* spell_buffer = state.spell_buffer();
   if(not spell_buffer->empty() && utils::has_value(*play_buffer)) {
      throw std::logic_error(
         "Both buffers for spells and fieldcards hold values. This should not occur.");
   }

   if(utils::has_value(*play_buffer)) {
      // the player cancelled playing this field spell so undo all targeting for its effects
      reset_targets(play_buffer->value()->effects(events::EventLabel::PLAY));
      play_buffer->reset();
   } else if(not spell_buffer->empty()) {
      // a spell to play with targeting was cancelled so cancel its targets
      reset_targets(spell_buffer->back()->effects(events::EventLabel::CAST));
      spell_buffer->pop_back();
   }

   return false;  // initiative stays with current player
}
bool actions::TargetingAction::execute_impl(GameState& state)
{
   // only set the chosen targets to the last element on the targeting targeting_buffer, which is
   // the one that requested targets
   auto* targeting_buffer = state.targeting_buffer();
   targeting_buffer->back()->targets(m_targets);
   auto assoc_card = targeting_buffer->back()->associated_card();
   if(targeting_buffer->size() == 1) {  // effect to choose targets for is last in buffer
      if(assoc_card->is_spell() &&  // effect belongs to a spell
         assoc_card->has_any_keyword({Keyword::BURST, Keyword::FOCUS})) {
         // if the effect to target is the last one in the targeting_buffer, and the effects belong
         // to a BURST or FOCUS spell, then a placing with subsequent targeting also triggers
         // playing it
         state.action_buffer()->emplace_back(
            std::make_shared< Action >(PlaySpellFinishAction(team(), true)));
      } else if(assoc_card->is_fieldcard()) {
         if(not state.action_buffer()->back()->is_play_finish()) {
            // if no replace action has occured before, then we have to place a PlayFinishAction
            state.action_buffer()->emplace_back(std::make_shared< Action >(
               PlayFieldCardFinishAction(team(), state.board()->camp(team())->size())));
         }
      }
   }
   // remove the last effect from the targeting targeting_buffer, since we just chose its targets
   targeting_buffer->pop_back();
   if(targeting_buffer->empty()) {
      state.logic()->restore_previous_invoker();
   }
   auto* hand = state.player(team()).hand();
   hand->erase(std::find(hand->begin(), hand->end(), assoc_card));
   return false;
}

bool actions::AcceptAction::execute_impl(GameState& state)
{
   // check if units have been placed on the battlefield and thus an attack or block was
   // commenced
   if(auto* bf_buffer = state.bf_buffer(); not bf_buffer->empty()) {
      bf_buffer->clear();
      auto& attack_token = state.player(team()).flags()->attack_token;
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
   } else if(not state.player(team()).flags()->has_played) {
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
bool actions::PlayAction::execute_impl(GameState& state)
{
   return true;
}

bool actions::PlayRequestAction::execute_impl(GameState& state)
{
   auto field_card = to_fieldcard(state.player(team()).hand()->at(m_hand_index));
   state.play_buffer()->emplace(field_card);
   auto* camp = state.board()->camp(team());
   if(camp->size() == state.board()->max_size_camp()) {
      // we need to choose the unit we want to replace, since the camp is full
      state.logic()->transition_action_invoker< ReplacingModeInvoker >();
      return false;
   }
   state.action_buffer()->emplace_back(
      std::make_shared< Action >(PlayFieldCardFinishAction(team(), camp->size())));

   if(field_card->has_effect(events::EventLabel::PLAY)) {
      for(const auto& effect : field_card->effects(events::EventLabel::PLAY)) {
         if(auto& targeter = *effect->targeter(); targeter.is_automatic()) {
            targeter(state, team());
         } else {
            state.targeting_buffer()->emplace_back(effect);
         }
      }
      if(not state.targeting_buffer()->empty()) {
         // set the next invoker to be a target mode invoker so that targets are chosen for the
         // effects in the buffer
         state.logic()->transition_action_invoker< TargetModeInvoker >();
         return false;
      }
   }
   return false;
}
bool actions::PlaySpellFinishAction::execute_impl(GameState& state)
{
   auto* spell_buffer = state.spell_buffer();
   // remove the spell to play from the spell buffer stack
   state.player(team()).flags()->has_played = true;
   while(not spell_buffer->empty()) {
      auto spell = spell_buffer->back();
      spell_buffer->pop_back();

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
bool actions::PlayFieldCardFinishAction::execute_impl(GameState& state)
{
   auto field_card = state.play_buffer()->value();
   state.play_buffer()->reset();
   state.player(team()).flags()->has_played = true;

   field_card->uncover();
   state.logic()->spend_mana(field_card);
   state.logic()->place_in_camp(field_card, m_camp_index);
   state.logic()->play_event_triggers(field_card);
   return true;
}

bool actions::ChoiceAction::execute_impl(GameState& state)
{
   // put choice at first place of the vector buffer
   auto* buffer = state.choice_buffer();
   buffer->begin()->swap(*std::next(buffer->begin(), m_choice));
   buffer->erase(std::next(buffer->begin(), 1), buffer->end());
   return false;
}
