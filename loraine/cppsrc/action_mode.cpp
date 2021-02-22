//
#include "engine/action_mode.h"
//
//bool Game::_do_action(const sptr< Action >& action)
//{
//   auto action_type = action->action_type();
//   bool flip_initiative = true;  // whether the other team gets to act next.
//   if(action_type == ActionType::PASS) {
//      // this case has to be handled separately to combine the pass counter
//      // reset for all other cases
//      bool round_ends = m_state->pass();
//      if(round_ends) {
//         _end_round();
//         _start_round();
//      }
//
//   } else {
//      m_state->reset_pass();
//
//      switch(action_type) {
//         case PLAY: {
//            auto cast_action = std::dynamic_pointer_cast< PlayAction >(action);
//            auto card = cast_action->card_played();
//            if(card->immutables().card_type == CardType::SPELL) {
//               auto floating = to_spell(card);
//               play_spells();
//               if(floating->has_keyword(Keyword::BURST)) {
//                  _resolve_spell_stack(true);
//                  flip_initiative = false;
//               }
//            } else {
//               play(card, cast_action->replacements());
//            }
//         }
//
//         case MOVE_UNIT: {
//            auto cast_action = std::dynamic_pointer_cast< MoveUnitAction >(action);
//            auto team = cast_action->team();
//            auto to_bf = cast_action->to_bf();
//
//            auto positions_own = cast_action->get_indices_vec();
//            auto positions_opp = cast_action->get_opp_indices_map();
//            _move_units(positions_own, team, false);
//            _move_units_opp(positions_opp, Team(1 - team), to_bf);
//            flip_initiative = false;
//         }
//
//         case MOVE_SPELL: {
//            auto cast_action = std::dynamic_pointer_cast< MoveSpellAction >(action);
//            auto floating = cast_action->spell();
//            if(not _move_spell(floating, cast_action->to_stack())) {
//               // the floating was supposed to be moved to the stack, but the team cancelled
//               _move_spell(floating, false);
//               flip_initiative = false;
//            } else {
//               flip_initiative = _do_action(std::make_shared< PlayAction >(
//                  m_state->round(), floating->mutables().owner, floating));
//            }
//         }
//
//         case ATTACK: {
//            auto team = action->team();
//            _activate_battlemode(team);
//            process_camp_queue(team);
//         }
//
//         case BLOCK: {
//            auto team = action->team();
//            _trigger_event(events::BlockEvent(team));
//         }
//
//         case ACCEPT: {
//            if(auto& spell_prestack = m_state->spell_prestack(); spell_prestack.empty()) {
//               _resolve_spell_stack(false);
//            } else {
//            }
//            if(m_state->in_battle_mode()) {
//               _resolve_battle();
//               _deactivate_battlemode();
//            }
//         }
//
//         default: break;
//      }
//   }
//   m_state->commit_to_history(action);
//   return flip_initiative;
//}