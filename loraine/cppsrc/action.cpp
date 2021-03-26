
#include "core/action.h"

#include "core/logic.h"
#include "core/state.h"

bool actions::PlaceSpellAction::execute_impl(State& state)
{
   auto reset_targets = [](std::vector< sptr< EffectBase > >& effects) {
      for(auto& effect : effects) {
         effect->reset_targets();
      }
   };
   auto* hand = state.player(m_spell->mutables().owner).hand();
   auto& effects_to_cast = m_spell->effects(events::EventLabel::CAST);
   auto* spell_stack = state.spell_stack();
   auto* spell_buffer = state.spell_buffer();
   if(m_to_stack) {
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
      // add the spell to the stack and the buffer, this would allow the
      // opponent to also see that a spell might be played
      hand->erase(std::find(hand->begin(), hand->end(), m_spell));
      spell_buffer->emplace_back(m_spell);
      spell_stack->emplace_back(m_spell);
      m_spell->move(Location::SPELLSTACK, spell_stack->size() - 1);
      if(state.targeting_buffer()->empty()
         && (m_spell->has_keyword(Keyword::BURST) || m_spell->has_keyword(Keyword::FOCUS))) {
         state.logic()->play_spell(m_spell);
      }
   } else {
      reset_targets(effects_to_cast);
      spell_buffer->erase(
         std::remove(spell_buffer->begin(), spell_buffer->end(), m_spell), spell_buffer->end());
      spell_stack->erase(
         std::remove(spell_stack->begin(), spell_stack->end(), m_spell), spell_stack->end());
      hand->emplace_back(m_spell);
      m_spell->move(Location::HAND, hand->size() - 1);
   }
   return false;
}

bool actions::PlaceUnitAction::execute_impl(State& state)
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
      utils::remove_by_indices(camp, m_indices_vec);
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
      utils::remove_by_indices(bf, m_indices_vec);
   }
}
bool actions::DragEnemyAction::execute_impl(State& state)
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
bool actions::MulliganAction::execute_impl(State& state)
{
   auto& hand = *state.player(state.active_team()).hand();
   auto& deck = *state.player(state.active_team()).deck();
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
         hand[i] = deck.draw_card();
      }
   }
}
bool actions::CancelAction::execute_impl(State& state)
{
   auto reset_targets = [](std::vector< sptr< EffectBase > >& effects) {
      for(auto& effect : effects) {
         effect->reset_targets();
      }
   };
   // the scenario of having a field card in the play buffer and a spell in the spell buffer is
   // logically not possible by the rules of the game. This would imply an implementation error
   auto* play_buffer = state.play_buffer();
   auto* spell_buffer = state.spell_buffer();
   if(not spell_buffer->empty() && utils::has_value(*play_buffer)) {
      throw std::logic_error(
         "Both buffers (spell and field card) hold values. This should not occur.");
   }

   if(utils::has_value(*play_buffer)) {
      // the player cancelled playing this field card so undo all targeting for its effects
      reset_targets(play_buffer->value()->effects(events::EventLabel::PLAY));
      play_buffer->reset();
   } else if(not spell_buffer->empty()) {
      reset_targets(spell_buffer->back()->effects(events::EventLabel::CAST));
      spell_buffer->pop_back();
   }

   return false;  // initiative stays with current player
}
bool actions::TargetingAction::execute_impl(State& state)
{
   // only set the chosen targets to the last element on the targeting buffer, which is the one that
   // requested targets
   auto* buffer = state.targeting_buffer();
   buffer->back()->targets(m_targets);
   if(auto assoc_card = buffer->back()->associated_card();
      buffer->size() == 1 &&  // the current effect to choose targets for is the last in the buffer
      assoc_card->is_spell() &&  // the effect belongs to a spell
      assoc_card->has_any_keyword({Keyword::BURST, Keyword::FOCUS})) {
      // if the effect to target is the last one in the buffer, and the effects belong to a BURST or
      // FOCUS spell, then that spell is immediately played
      auto spell = to_spell(assoc_card);
      state.logic()->play_spell(spell);
      state.logic()->cast(spell);
   }
   // remove the last effect from the targeting buffer, since we just chose its targets
   buffer->pop_back();
   return false;
}

bool actions::AcceptAction::execute_impl(State& state)
{
   if(auto* buffer = state.spell_buffer(); not buffer->empty()) {
      for(auto& spell : *buffer) {
         spell->uncover();
         state.logic()->play_spell(spell);
      }
      buffer->clear();
   } else if(auto* bf_buffer = state.bf_buffer(); not bf_buffer->empty()) {
      bf_buffer->clear();
      if(auto& attack_token = state.player(team()).flags()->attack_token;
         attack_token && not state.logic()->in_combat()) {
         // if the game is not already in combat mode and the player holds an attack token, then
         // having units in the bf_buffer must mean, that the player has just declared an attack
         attack_token = false;
         state.logic()->trigger_event< events::EventLabel::ATTACK >(team());
      } else if(not attack_token) {
         // reaching this point should mean that there is an active combat and the current player is
         // not the attacker, thus they moved units because they were asked to block. We are
         // ASSUMING here that the is_valid method correctly filters out actions of moving units at
         // all other times other than attack or block declarations
         // TODO: Branch kept alive for later logging here
      }
   } else if(
      state.spell_stack()->empty() && state.board()->battlefield(team())->empty()
      && state.board()->battlefield(opponent(team()))->empty()) {
      // if the entire board is resolved, then the player has simply passed initiative
      state.logic()->pass();
   } else {
      // the spell stack buffer is empty and no attack or block were declared, so whatever the
      // current board state is, has been accepted. Thus we go on to resolve the spell stack and any
      // open combat
      state.requires_resolution(true);
   }

   return true;
}
