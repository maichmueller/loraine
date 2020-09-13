
#include "game.h"

#include <event/active_event.h>

#include <utility>

bool Game::run_game()
{
   // decide the player who starts attacking
   unsigned short starting_player = std::uniform_int_distribution< size_t >(
      0, 1)(rng::rng_def_engine);
   m_state->set_starting_player(PLAYER(starting_player));

   // draw the starting cards for each player and let them _mulligan
   auto hand_blue = _draw_initial_hand(PLAYER::BLUE);
   auto hand_red = _draw_initial_hand(PLAYER::RED);
   _mulligan(hand_blue, hand_red);

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
         bool incr_turn = _do_action(action);
         if(incr_turn) {
            m_state->incr_turn();
         }
      }

      if(auto terminality = m_state->is_terminal() != ONGOING) {
         return terminality;
      }
   }
}

bool Game::_do_action(const sptr< AnyAction >& action)
{
   auto action_type = action->get_action_type();
   if(action_type == ActionType::PASS) {
      // this case has to be handled separately to combine the pass counter
      // reset for all other cases
      m_state->incr_pass_count();
      m_state->incr_turn();
   } else {
      m_state->reset_pass_count();

      switch(action_type) {
         case PLAY: {
            auto cast_action = std::dynamic_pointer_cast< PlayAction >(action);
            auto [player, round, _, card] = cast_action->get_action_data();
            if(card->get_card_type() == CardType::SPELL) {
               auto spell = std::dynamic_pointer_cast< Spell >(card);
               m_state->play_spell(spell);
               if(! spell->has_keyword(Keyword::BURST)) {
                  return false;
               }
            } else {
               m_state->play_unit(std::dynamic_pointer_cast< Unit >(card));
            }
         }

         case ATTACK: {
            auto cast_action = std::dynamic_pointer_cast< AttackAction >(
               action);
            auto
               [player,
                round,
                action_type,
                positions,
                opt_spells] = cast_action->get_action_data();
            m_state->get_board()->move_to_battlefield(positions, player);
            if(opt_spells.has_value()) {
               for(const auto& spell : opt_spells.value()) {
                  m_state->play_spell(spell);
               }
               _activate_battlemode(player, positions);
            }
         }

         case BLOCK: {
            auto cast_action = std::dynamic_pointer_cast< BlockAction >(action);
            auto
               [player,
                round,
                action_type,
                positions,
                opt_spells] = cast_action->get_action_data();
            m_state->get_board()->move_to_battlefield(positions, player);

            if(opt_spells.has_value()) {
               for(const auto& spell : opt_spells.value()) {
                  m_state->play_spell(spell);
               }
            }
         }

         case ACCEPT: {
            _resolve_spell_stack();
            if(m_battle_mode) {
               _resolve_battle();
            }
         }

         default: break;
      }
   }
   return true;
}
void Game::_activate_battlemode(
   PLAYER attack_player, std::vector< size_t > positions)
{
   m_battle_mode = true;
   m_state->set_attacker(attack_player);
   events::trigger(events::AttackEvent(attack_player, std::move(positions)));
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
   auto defender = PLAYER(1 - attacker);
   auto& battlefield_att = m_state->get_board()->get_battlefield(attacker);
   auto& battlefield_def = m_state->get_board()->get_battlefield(defender);
   for(auto pos = 0; pos < BATTLEFIELD_SIZE; ++pos) {
      auto unit_att_opt = battlefield_att.at(pos);

      if(unit_att_opt.has_value()) {
         // if this spot on the battlefield sees an actual unit posted for
         // attack we need to make it strike
         auto unit_att = std::dynamic_pointer_cast< Unit >(
            unit_att_opt.value());

         auto unit_def_opt = battlefield_def.at(pos);
         if(unit_def_opt.has_value()) {
            // if there is a defender posted on the same spot of the
            // attacker, it needs to battle the attacker
            auto unit_def = std::dynamic_pointer_cast< Unit >(
               unit_def_opt.value());

            if(unit_def->is_alive()) {
               bool quick_attacks = unit_att->has_keyword(
                  Keyword::QUICK_ATTACK);

               int health_att_after = 0;
               int health_def_after = 0;

               sptr< size_t > def_power = std::make_shared< size_t >(
                  unit_def->get_power());

               if(quick_attacks) {
                  _strike_unit(attacker, defender, unit_att, unit_def);
                  if(unit_def->get_health() < 1) {
                     // we cant check for the unit being alive, but have to
                     // check via the health instead, because the unit is
                     // only killed later on. The health is an intermediary.
                     _strike_unit(defender, attacker, unit_def, unit_att);
                  }
               } else {
                  _strike_unit(attacker, defender, unit_att, unit_def);
                  _strike_unit(defender, attacker, unit_def, unit_att);
               }

               // check whether to kill the attacking unit
               if(unit_att->get_health() < 1) {
                  _kill_unit(defender, unit_att);
               }
               // check whether to kill defending unit and do overwhelm
               // damage
               if(unit_def->get_health() < 1) {
                  _kill_unit(attacker, unit_def);

                  if(unit_att->has_keyword(Keyword::OVERWHELM)) {
                     sptr< size_t > overwhelm_dmg = std::make_shared< size_t >(
                        -health_def_after);

                     events::trigger(events::NexusStrikeEvent(
                        defender, attacker, overwhelm_dmg, unit_att, true));

                     _nexus_hit(defender, attacker, overwhelm_dmg, unit_att);
                  }
               }
            }
         } else {
            // if there is no defender to block the attack, the attacker
            // strikes the nexus
            sptr< size_t > att_power = std::make_shared< size_t >(
               unit_att->get_power());

            if(*att_power > 0) {
               events::trigger(events::StrikeEvent(attacker, unit_att));
               _nexus_hit(defender, attacker, att_power, unit_att);
            }
         }
      }
      m_battle_mode = false;
   }
}
int Game::_strike_unit(
   const PLAYER& attacker,
   const PLAYER& defender,
   const std::shared_ptr< Unit >& unit_att,
   std::shared_ptr< Unit >& unit_def) const
{
   sptr< size_t > att_power = std::make_shared< size_t >(unit_att->get_power());

   if(*att_power > 0) {
      events::trigger(events::StrikeEvent(attacker, unit_att));
      return _deal_damage_to_unit(defender, unit_def, att_power);
   }
}
int Game::_deal_damage_to_unit(
   const PLAYER& player_of_unit,
   std::shared_ptr< Unit >& unit,
   const sptr< size_t >& damage) const
{
   events::trigger(events::UnitTakeDamageEvent(player_of_unit, unit, damage));

   return unit->add_health(-*damage);
}
void Game::_kill_unit(PLAYER killer, const std::shared_ptr< Unit >& unit)
{
   events::trigger(events::DieEvent(killer, unit));
   unit->set_flag_alive(false);
}

void Game::_nexus_hit(
   PLAYER attacked_nexus,
   PLAYER attacking_player,
   const sptr< size_t >& damage,
   std::shared_ptr< Card > responsible_card)
{
   events::trigger(events::NexusStrikeEvent(
      attacked_nexus,
      attacking_player,
      damage,
      std::move(responsible_card),
      true));

   m_state->set_nexus_health(
      m_state->get_nexus_health(attacked_nexus) - *damage, attacked_nexus);
}

std::vector< sptr< Card > > Game::_draw_initial_hand(PLAYER player)
{
   return m_state->draw_n_cards(player, INITIAL_HAND_SIZE);
}
void Game::_mulligan(
   const std::vector< sptr< Card > >& hand_blue,
   const std::vector< sptr< Card > >& hand_red)
{
   std::array< std::vector< sptr< Card > >, 2 > hands{hand_blue, hand_red};

   auto mull_act_blue = m_agents[PLAYER::BLUE]->decide_mulligan(
      *m_state, hand_blue);
   auto mull_act_red = m_agents[PLAYER::RED]->decide_mulligan(
      *m_state, hand_red);

   std::array< std::array< bool, INITIAL_HAND_SIZE >, 2 > replace = {
      mull_act_blue->get_replace_decisions(),
      mull_act_red->get_replace_decisions()};

   for(int p = PLAYER::BLUE; p != PLAYER::RED; ++p) {
      auto& curr_hand = hands[p];
      State::HandType new_hand;
      auto nr_cards_to_replace = 0;
      auto replace_for_p = replace.at(p);
      auto hand_size = curr_hand.size();
      for(auto i = 0; i < hand_size; ++i) {
         if(replace_for_p.at(i)) {
            m_state->shuffle_card_into_deck(curr_hand.at(i), PLAYER(p));
            nr_cards_to_replace += 1;
         }
      }
      auto new_cards = m_state->draw_n_cards(PLAYER(p), nr_cards_to_replace);
      for(size_t i = 0; i < hand_size; ++i) {
         if(replace_for_p.at(i)) {
            new_hand.at(i) = new_cards.back();
            new_cards.pop_back();
         } else {
            new_hand.at(i) = curr_hand.at(i);
         }
      }
      m_state->set_hand(new_hand, PLAYER(p));
   }

}
