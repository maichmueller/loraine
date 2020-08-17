
#include "game.h"

#include <event/active_event.h>

bool Game::run_game()
{
   // decide the player who starts attacking
   unsigned short starting_player = std::uniform_int_distribution< size_t >(
      0, 1)(rng::rng_def_engine);
   m_state->set_starting_player(PLAYER(starting_player));

   // draw the starting cards for each player and let them _mulligan
   _draw_initial_hand(PLAYER::BLUE);
   _draw_initial_hand(PLAYER::RED);
   _mulligan();

   // the actual run of the game. We play until the state says it is in a
   // terminal state. Then we return out out of the method
   while(true) {
      _start_round();

      while(true) {
         // check for breaking the action loop
         if(const auto& hist_ptr = *m_state->get_history();
            hist_ptr.size() > 1  // if we didnt't check this we'd get a segfault
            && (*(std::rbegin(hist_ptr)))->get_action_type()
                  == ActionType::PASS  // the ultimate action was a pass
            && (*(std::rbegin(hist_ptr) + 1))->get_action_type()
                  == ActionType::PASS) {  // the penultimate action was a pass

            // both players passed, therefore the round is over
            break;
         }

         // ask current agent for action
         PLAYER turn = m_state->get_turn();
         auto action = m_agents[turn].decide_action(*m_state);
         m_state->do_action(action);
      }

      if(auto terminality = m_state->is_terminal() != ONGOING) {
         return terminality;
      }
   }
}

void Game::_do_action(const sptr< Action >& action)
{
   auto action_type = action->get_action_type();
   switch(action_type) {
      case PASS: break;
      case PLAY: {
         auto cast_action = std::dynamic_pointer_cast< PlayAction >(action);
         auto [player, round, _, card] = cast_action->get_action_data();
         if(card->get_card_type() == CardType::SPELL) {
            m_state->play_spell(card);
         } else {
            m_state->play_unit(card);
         }
      }
      case ATTACK: {
         auto cast_action = std::dynamic_pointer_cast< AttackAction >(action);
         auto [player, round, _, positions] = cast_action->get_action_data();
         m_state->get_board()->move_to_battlefield(positions, player);

         _activate_battlemode(player, true);
      }

      case BLOCK: break;
      case ROUND_END: break;
      case ACCEPT: {
         _resolve_spell_stack();
         if(m_battle_mode) {
            _resolve_battle();
         }
      }
      case MULLIGAN: break; m_state->incr_turn();
   }
   return action_type;
}
void Game::_activate_battlemode(PLAYER attack_player, bool battlemode_on)
{
   m_battle_mode = battlemode_on;
   m_state->set_attacker(attack_player);
   events::active_event::set(events::BattleEvent());
}
void Game::_resolve_spell_stack()
{
   auto& spell_stack = m_state->get_spell_stack();
   while(! spell_stack.empty()) {
      auto last_spell = *spell_stack.back();
      spell_stack.pop_back();
      // spells dont react to events particularly, so they are passed a None-
      // event
      last_spell(*m_state, events::AnyEvent());
   }
}
void Game::_resolve_battle()
{
   auto attacker = m_state->get_attacker().value();
   auto& battlefield_att = m_state->get_board()->get_battlefield(attacker);
   auto& battlefield_def = m_state->get_board()->get_battlefield(
      PLAYER(1 - attacker));
   for(auto pos = 0; pos < BATTLEFIELD_SIZE; ++pos) {
      auto unit_att_opt = battlefield_att.at(pos);

      if(unit_att_opt.has_value()) {
         // if this spot on the battlefield sees an actual unit posted for
         // attack we need to make it strike
         auto unit_att = std::dynamic_pointer_cast< Unit >(
            unit_att_opt.value());
         auto unit_def_opt = battlefield_def.at(pos);
         if(unit_def_opt.has_value()) {
            // if there is a defender posted on the same spot of the attacker,
            // it needs to battle the attacker
            auto unit_def = std::dynamic_pointer_cast< Unit >(
               unit_def_opt.value());

            auto att_kwords = unit_att->get_keywords();
            bool quick_attacks = std::find(
                                    att_kwords.begin(),
                                    att_kwords.end(),
                                    Keyword::QUICK_ATTACK)
                                 != att_kwords.end();
            bool overwhelms = std::find(
                                 att_kwords.begin(),
                                 att_kwords.end(),
                                 Keyword::OVERWHELM)
                              != att_kwords.end();
            if(quick_attacks) {
               auto attack_damage = -unit_att->get_attack();
               bool def_is_damageable = unit_def->is_damageable(
                  unit_att, attack_damage);
               std::optional<int> health_def;
               if(def_is_damageable) {
                  health_def = unit_def->add_health(attack_damage);
                  events::active_event::set(events::UnitTakeDamageEvent(
                     attacker, unit_def, attack_damage));
               }

               // activate the strike event now
               events::active_event::set(
                  events::StrikeEvent(attacker, unit_att->get_uuid()));

               if(health_def.has_value()) {
                  events::active_event::set(events::UnitTakeDamageEvent())
                  _kill(unit_def)
               }
            }
            unit_att, unit_def_opt, quick_attacks);
         }
      }
   }
}
