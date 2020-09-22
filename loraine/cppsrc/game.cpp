
#include "game.h"

#include <event/active_event.h>

#include <utility>

#include "rulesets.h"

bool Game::run_game()
{
   // decide the player who starts attacking
   PLAYER starting_player = PLAYER(
      std::uniform_int_distribution< size_t >(0, 1)(rng::rng_def_engine));
   m_state->set_starting_player(starting_player);

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
         if(m_state->get_pass_count() == 2) {
            // both players passed, therefore the round is over
            break;
         }
         // ask current agent for action
         PLAYER turn = m_state->get_turn();
         auto action = m_agents[turn]->decide_action(*m_state);

         if(_do_action(action)) {
            m_state->incr_turn();
         }

         if(auto terminality = m_state->is_terminal() != ONGOING) {
            return terminality;
         }
      }
      _end_round();
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
                positions_own,
                positions_opp,
                opt_spells] = cast_action->get_action_data();
            m_state->get_board()->move_to_battlefield(positions_own, player);
            m_state->get_board()->move_to_battlefield(positions_opp, player);
            if(opt_spells.has_value()) {
               for(const auto& spell : opt_spells.value()) {
                  m_state->play_spell(spell);
               }
               _activate_battlemode(player, positions_own);
            }
            // remove the attack token of the attacker, as it is now used up
            m_state->set_flag_can_attack(false, player);
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
      last_spell(*this, events::AnyEvent());
   }
}
void Game::_resolve_battle()
{
   auto attacker = m_state->get_attacker().value();
   auto defender = PLAYER(1 - attacker);
   auto& battlefield_att = m_state->get_board()->get_battlefield(attacker);
   auto& battlefield_def = m_state->get_board()->get_battlefield(defender);
   for(auto pos = 0U; pos < BATTLEFIELD_SIZE; ++pos) {
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
               bool double_attacks = unit_att->has_keyword(
                  Keyword::DOUBLE_ATTACK);

               int health_def_after = 0;

               sptr< size_t > def_power = std::make_shared< size_t >(
                  unit_def->get_power());

               if(quick_attacks || double_attacks) {
                  _strike_unit(attacker, defender, unit_att, unit_def);
                  if(unit_def->get_health() > 0) {
                     // we cant check for the unit being alive, but have to
                     // check via the health instead, because the unit is
                     // only killed later on. The health is an intermediary.
                     if(double_attacks) {
                        // a double attacking unit attacks again after a quick
                        // attack
                        _strike_unit(attacker, defender, unit_att, unit_def);
                     }
                     _strike_unit(defender, attacker, unit_def, unit_att);
                  }

               } else {
                  _strike_unit(attacker, defender, unit_att, unit_def);
                  _strike_unit(defender, attacker, unit_def, unit_att);
               }

               // check whether to kill the attacking unit
               if(unit_att->get_health() < 1) {
                  _kill_unit(defender, unit_att, unit_att);
               }
               // check whether to kill defending unit and do overwhelm
               // damage
               if(unit_def->get_health() < 1) {
                  _kill_unit(attacker, unit_def, unit_att);

                  if(unit_att->has_keyword(Keyword::OVERWHELM)) {
                     sptr< size_t > overwhelm_dmg = std::make_shared< size_t >(
                        -health_def_after);

                     events::trigger(events::NexusStrikeEvent(
                        defender, attacker, overwhelm_dmg, unit_att, true));

                     _nexus_strike(defender, attacker, overwhelm_dmg, unit_att);
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
               _nexus_strike(defender, attacker, att_power, unit_att);
            }
         }
      }
      m_battle_mode = false;
   }
}
long int Game::_strike_unit(
   PLAYER attacker,
   PLAYER defender,
   const std::shared_ptr< Unit >& unit_att,
   std::shared_ptr< Unit >& unit_def) const
{
   sptr< size_t > att_power = std::make_shared< size_t >(unit_att->get_power());

   if(*att_power > 0) {
      events::trigger(events::StrikeEvent(attacker, unit_att));
      return _deal_damage_to_unit(defender, unit_def, att_power);
   }
   return unit_def->get_health();
}
long int Game::_deal_damage_to_unit(
   PLAYER unit_owner,
   std::shared_ptr< Unit >& unit,
   const sptr< size_t >& damage) const
{
   events::trigger(events::UnitTakeDamageEvent(unit_owner, unit, damage));
   unit->take_damage(*damage);
   return unit->get_health();
}
void Game::_kill_unit(
   PLAYER killer,
   const std::shared_ptr< Unit >& killed_unit,
   const std::shared_ptr< Card >& killing_card)
{
   killed_unit->die(*this);
   events::trigger(events::DieEvent(killer, killed_unit, killing_card));
   if(! killed_unit->is_alive()) {
      // we need to check for the card being truly dead, in case it had an
      // e.g. last breath effect, which kept it alive
      m_state->move_to_graveyard(killed_unit, killed_unit->get_owner());
   }
   if(! m_battle_mode) {
      m_state->get_board()->reorganize_camp(killed_unit->get_owner());
   }
}

void Game::_nexus_strike(
   PLAYER attacked_nexus,
   PLAYER attacking_player,
   const sptr< size_t >& damage,
   const std::shared_ptr< Card >& responsible_card,
   bool strike_during_combat)
{
   events::trigger(events::NexusStrikeEvent(
      attacked_nexus,
      attacking_player,
      damage,
      responsible_card,
      strike_during_combat));

   m_state->damage_nexus(*damage, attacked_nexus);
   if(strike_during_combat) {
      if(auto unit = to_unit(responsible_card); unit->has_keyword(Keyword::LIFESTEAL)) {
         lifesteal(unit, *damage);
      }
   }
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
      auto& curr_hand = hands.at(p);
      State::HandType new_hand;
      auto nr_cards_to_replace = 0;
      auto replace_for_p = replace.at(p);
      auto hand_size = curr_hand.size();
      for(auto i = 0U; i < hand_size; ++i) {
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
void Game::_end_round()
{
   events::trigger(events::RoundEndEvent(m_state->get_round()));
   for(int p = 0; p != 1; ++p) {
      auto player = PLAYER(p);
      // undo temporary buffs/nerfs and possibly heal the units if applicable
      for(auto opt_unit : m_state->get_board()->get_camp(player)) {
         if(opt_unit.has_value()) {
            auto unit = opt_unit.value();
            for(const auto& grant : unit->get_grants_map()) {
               if(! grant.second->is_permanent()) {
                  grant.second->remove();
               }
            }
            if(unit->has_keyword(Keyword::REGENERATION) && unit->is_damaged()) {
               unit->regenerate();
            }
         }
      }
      // float spell mana if available
      auto remaining_mana = m_state->get_mana(player);
      m_state->set_spell_mana(
         m_state->get_spell_mana(player) + remaining_mana, player);
   }
}
void Game::_start_round()
{
   m_state->incr_round();
   m_state->set_flag_can_plunder(false, PLAYER::BLUE);
   m_state->set_flag_can_plunder(false, PLAYER::RED);
   auto attacker = PLAYER(
      m_state->get_starting_player() + m_state->get_round() % 2);
   m_state->set_flag_can_attack(attacker == PLAYER::BLUE, PLAYER::BLUE);
   m_state->set_flag_can_attack(attacker == PLAYER::RED, PLAYER::RED);

   events::trigger(events::RoundStartEvent(m_state->get_round()));
   for(int player = PLAYER::BLUE; player != PLAYER::RED; ++player) {
      incr_managems(PLAYER(player));
   }
   m_state->fill_mana(PLAYER::BLUE);
   m_state->fill_mana(PLAYER::RED);
}
void Game::incr_managems(PLAYER player, size_t amount)
{
   m_state->incr_managems(player, amount);
   _check_enlightenment(player);
}

void Game::_check_enlightenment(PLAYER player)
{
   if(m_state->is_enlightened(player)) {
      events::trigger(events::EnlightenmentEvent(player));
   }
}
void Game::decr_managems(PLAYER player, size_t amount)
{
   m_state->decr_managems(player, amount);
}
void Game::play(sptr< Unit > unit)
{
   auto& camp = m_state->get_board()->get_camp(unit->get_owner());
   for()
}
