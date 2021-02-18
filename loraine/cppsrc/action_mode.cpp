
#include "engine/action_mode.h"

bool Game::_do_action(const sptr< Action >& action)
{
   auto action_type = action->get_action_type();
   bool flip_initiative = true;  // whether the other player gets to act next.
   if(action_type == ActionType::PASS) {
      // this case has to be handled separately to combine the pass counter
      // reset for all other cases
      bool round_ends = m_state->pass();
      if(round_ends) {
         _end_round();
         _start_round();
      }

   } else {
      m_state->reset_pass();

      switch(action_type) {
         case PLAY: {
            auto cast_action = std::dynamic_pointer_cast< PlayAction >(action);
            auto card = cast_action->get_card_played();
            if(card->immutables().card_type == CardType::SPELL) {
               auto spell = to_spell(card);
               play_spells();
               if(spell->has_keyword(Keyword::BURST)) {
                  _resolve_spell_stack(true);
                  flip_initiative = false;
               }
            } else {
               play(card, cast_action->get_replace_idx());
            }
         }

         case MOVE_UNIT: {
            auto cast_action = std::dynamic_pointer_cast< MoveUnitAction >(action);
            auto player = cast_action->get_player();
            auto to_bf = cast_action->towards_bf();

            auto positions_own = cast_action->get_indices_vec();
            auto positions_opp = cast_action->get_opp_indices_map();
            _move_units(positions_own, player, false);
            _move_units_opp(positions_opp, Player(1 - player), to_bf);
            flip_initiative = false;
         }

         case MOVE_SPELL: {
            auto cast_action = std::dynamic_pointer_cast< MoveSpellAction >(action);
            auto spell = cast_action->get_spell();
            if(not _move_spell(spell, cast_action->towards_stack())) {
               // the spell was supposed to be moved to the stack, but the player cancelled
               _move_spell(spell, false);
               flip_initiative = false;
            } else {
               flip_initiative = _do_action(std::make_shared< PlayAction >(
                  m_state->round(), spell->mutables().owner, spell));
            }
         }

         case ATTACK: {
            auto player = action->get_player();
            _activate_battlemode(player);
            process_camp_queue(player);
         }

         case BLOCK: {
            auto player = action->get_player();
            _trigger_event(events::BlockEvent(player));
         }

         case ACCEPT: {
            if(auto& spell_prestack = m_state->spell_prestack(); spell_prestack.empty()) {
               _resolve_spell_stack(false);
            } else {
            }
            if(m_state->in_battle_mode()) {
               _resolve_battle();
               _deactivate_battlemode();
            }
         }

         default: break;
      }
   }
   m_state->commit_to_history(action);
   return flip_initiative;
}