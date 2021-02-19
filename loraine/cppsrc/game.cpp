
#include "game.h"

#include <algorithms.h>

#include <utility>

#include "engine/rulesets.h"
#include "grants/grant.h"
#include "utils/utils.h"

void Game::_move_units(const std::vector< size_t >& positions, Player player, bool to_bf)
{
   auto min_target_pos = m_state->m_board->count_units(player, not to_bf);
   auto& battlefield = m_state->m_board->get_battlefield(player);
   auto& camp = m_state->m_board->get_camp(player);
   if(to_bf) {
      for(size_t i = 0; i < positions.size(); ++i) {
         size_t source_pos = positions.at(i);
         size_t target_pos = i + min_target_pos;
         auto& unit = camp.at(source_pos);
         unit->move(Location::BATTLEFIELD, target_pos);
         battlefield.at(target_pos) = to_unit(unit);
         camp.erase(camp.begin() + source_pos);
      }
   } else {
      for(size_t i = 0; i < positions.size(); ++i) {
         size_t source_pos = positions.at(i);
         size_t target_pos = i + min_target_pos;
         auto& unit = battlefield.at(source_pos);
         throw_if_no_value(unit);
         unit->move(Location::CAMP, target_pos);
         camp.at(target_pos) = unit;
         reset(unit);
      }
   }
}
void Game::_move_units_opp(const std::map< size_t, size_t >& positions, Player player, bool to_bf)
{
   auto& battlefield = m_state->m_board->get_battlefield(player);
   auto& camp = m_state->m_board->get_camp(player);

   std::function< void(size_t, size_t) > move_lambda;
   if(to_bf) {
      auto& bf_opp = m_state->m_board->get_battlefield(Player(1 - player));
      move_lambda = [&camp, &battlefield, bf_opp](size_t source_pos, size_t target_pos) {
         auto& unit = camp.at(source_pos);
         const auto& unit_opp = bf_opp.at(target_pos);
         throw_if_no_value(unit_opp);
         if(not unit->has_keyword(Keyword::CHALLENGER)
            && not unit_opp->has_keyword(Keyword::VULNERABLE)) {
            throw std::logic_error(
               "A unit was dragged which was neither VULNERABLE nor had been challenged by a "
               "CHALLENGER.");
         }
         // move the unit to the battlefield now
         unit->move(Location::BATTLEFIELD, target_pos);
         battlefield.at(target_pos) = to_unit(unit);
         // remove the moved unit from the camp
         auto camp_iter = camp.begin();
         std::advance(camp_iter, source_pos);
         camp.erase(camp_iter);
      };
   } else {
      move_lambda = [&](size_t source_pos, size_t target_pos) {
         auto& unit = battlefield.at(source_pos);
         throw_if_no_value(unit);
         unit->move(Location::CAMP, target_pos);
         camp.at(target_pos) = unit;
         reset(unit);
      };
   }

   for(const auto& pos : positions) {
      move_lambda(pos.first, pos.second);
   }
}

bool Game::_move_spell(const sptr< Spell >& spell, bool to_stack)
{
   Player player = spell->mutables().owner;
   auto hand = m_state->hand(player);
   auto& cast_effects = spell->effects(events::EventType::CAST);
   if(to_stack) {
      hand.erase(std::find(hand.begin(), hand.end(), spell));
      auto& spell_pre_stack = m_state->spell_prestack();
      spell_pre_stack.emplace_back(spell);
      spell->move(Location::SPELLSTACK, spell_pre_stack.size() - 1);
      for(auto& effect : cast_effects) {
         if(not effect.target(*m_state, *agent(player), player)) {
            // the agent has cancelled playing the spell, we thus return handling to the action
            // method
            return false;
         }
      }
   } else {
      for(auto& effect : cast_effects) {
         effect.reset_targets();
      }
      auto& spell_stack = m_state->spell_prestack();
      spell_stack.erase(std::find(spell_stack.begin(), spell_stack.end(), spell));
      hand.emplace_back(spell);
      spell->move(Location::HAND, hand.size() - 1);
   }

   return true;
}

bool Game::run_game()
{
   // decide the player who starts attacking
   Player starting_player = Player(
      std::uniform_int_distribution< size_t >(0, 1)(rng::engine()));
   m_state->starting_player(starting_player);

   // draw the starting cards for each player and let them _mulligan
   auto hand_blue = _draw_n_cards(BLUE, INITIAL_HAND_SIZE);
   auto hand_red = _draw_n_cards(RED, INITIAL_HAND_SIZE);
   _mulligan(hand_blue, hand_red);

   // the actual run of the game. We play until the state says it is in a
   // terminal state.

   while(true) {
      // ask current agent for action
      Player turn = m_state->active_player();
      auto action = m_agents[turn]->decide_action(*m_state);

      if(_do_action(action)) {
         m_state->incr_turn();
      }

      if(auto terminality = m_state->status() != ONGOING) {
         return terminality;
      }
   }
}

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
void Game::_activate_battlemode(Player attack_player)
{
   m_state->set_battle_mode(true);
   m_state->attacker(attack_player);
   auto& bf = m_state->m_board->get_battlefield(attack_player);
   auto check_att_effects = [&](const sptr< Card >& unit) {
      if(unit->has_keyword(Keyword::ATTACK)) {
         _trigger_event(events::AttackEvent(attack_player, unit));
      }
   };
   auto check_supp_effects = [&](const sptr< Card >& unit, const sptr< Card >& next_unit) {
      if(unit->has_keyword(Keyword::SUPPORT)) {
         _trigger_event(events::SupportEvent(attack_player, unit, Target(next_unit)));
      }
   };

   for(int i = 0; i < BATTLEFIELD_SIZE - 1; ++i) {
      if(has_value(bf[i])) {
         const auto& attack_unit = bf[i];
         check_att_effects(attack_unit);
         if(auto next_unit = bf[i + 1]; has_value(next_unit)) {
            check_supp_effects(attack_unit, next_unit);
         }
      }
   }
   if(auto last_att = bf[BATTLEFIELD_SIZE]; has_value(last_att)) {
      check_att_effects(last_att);
   }

   // remove the attack token of the attacker, as it is now used up,
   // unless all units were scouts (for the first attack)
   bool scout_attack = true;
   if(! m_state->token_scout(attack_player)) {
      for(auto opt_unit : m_state->m_board->get_battlefield(attack_player)) {
         if(has_value(opt_unit)) {
            if(not opt_unit->has_keyword(Keyword::SCOUT)) {
               scout_attack = false;
               break;
            }
         }
      }
   } else {
      scout_attack = false;
   }
   if(scout_attack) {
      m_state->token_scout(attack_player, true);
      _trigger_event(events::ScoutEvent(attack_player));
   } else {
      m_state->token_attack(false, attack_player);
   }
}
void Game::_deactivate_battlemode()
{
   m_state->set_battle_mode(false);
   m_state->reset_attacker();
}
void Game::_resolve_spell_stack(bool burst)
{
   auto& spell_stack = m_state->spell_stack();
   if(burst) {
      auto last_spell = spell_stack.back();
      spell_stack.pop_back();
      cast(last_spell);
   } else {
      _cast_spellstack();
      if(m_state->in_battle_mode()) {
         _resolve_battle();
      }
   }
}
void Game::_resolve_battle()
{
   m_state->set_battle_resolution_mode(true);
   auto attacker = m_state->attacker().value();
   auto defender = Player(1 - attacker);
   auto& battlefield_att = m_state->m_board->get_battlefield(attacker);
   auto& battlefield_def = m_state->m_board->get_battlefield(defender);
   for(auto pos = 0U; pos < m_state->m_board->count_units(attacker, false); ++pos) {

      auto unit_att_opt = battlefield_att.at(pos);

      if(has_value(unit_att_opt)) {
         // if this spot on the battlefield sees an actual unit posted for
         // attack we need to make it strike
         auto unit_att = to_unit(unit_att_opt);

         auto unit_def_opt = battlefield_def.at(pos);
         if(has_value(unit_def_opt)) {
            // if there is a defender posted on the same spot of the
            // attacker, it needs to battle the attacker
            auto unit_def = to_unit(unit_def_opt);

            if(unit_def->unit_mutables().alive) {
               bool quick_attacks = unit_att->has_keyword(Keyword::QUICK_ATTACK);
               bool double_attacks = unit_att->has_keyword(Keyword::DOUBLE_ATTACK);

               long health_def_after = 0;
               long attack_power = unit_att->power();

               if(quick_attacks || double_attacks) {
                  strike(unit_att, unit_def);
                  if(unit_def->health() > 0) {
                     // we cant check for the unit being alive, but have to
                     // check via the health instead, because the unit is
                     // only killed later on. The health is an intermediary.
                     if(double_attacks) {
                        // a double attacking unit attacks again after a quick
                        // attack
                        strike(unit_att, unit_def);
                     }
                     strike(unit_def, unit_att);
                  }

               } else {
                  strike(unit_att, unit_def);
                  strike(unit_def, unit_att);
               }

               // check whether to kill the attacking unit
               if(unit_att->health() == 0) {
                  kill_unit(defender, unit_att, unit_att);
               }
               // check whether to kill defending unit
               if(unit_def->health() == 0) {
                  kill_unit(attacker, unit_def, unit_att);
               }
            }
         } else {
            // if there is no defender to block the attack, the attacker
            // strikes the nexus
            nexus_strike(defender, unit_att);
         }
      }
      retreat_to_camp(attacker);
      retreat_to_camp(defender);
      m_state->set_battle_mode(false);
   }
   m_state->set_battle_resolution_mode(false);
}
void Game::strike(const sptr< Unit >& unit_att, sptr< Unit >& unit_def)
{
   sptr< long > damage = std::make_shared< long >(unit_att->power());
   if(*damage > 0) {
      _trigger_event(events::StrikeEvent(unit_att->mutables().owner, unit_att, unit_def));
      deal_damage_to_unit(unit_att, unit_def, damage);
   }
   if(m_state->in_battle_mode() && m_state->attacker() == unit_att->mutables().owner
      && unit_att->has_keyword(Keyword::OVERWHELM)) {
      nexus_strike(unit_def->mutables().owner, damage, unit_att);
   }
}
void Game::deal_damage_to_unit(
   const sptr< Card >& cause, const sptr< Unit >& unit, const sptr< long >& damage)
{
   long health_def = unit->health();
   _trigger_event(
      events::UnitDamageEvent(cause->mutables().owner, cause, Target(unit), damage));
   unit->take_damage(*damage);
   // store any surplus damage in the damage ptr (e.g. for overwhelm dmg calc)
   *damage += -health_def;
}
void Game::kill_unit(Player killer, const sptr< Unit >& killed_unit, const sptr< Card >& cause)
{
   killed_unit->kill();
   _trigger_event(events::DieEvent(killer, Target(killed_unit), cause));
   if(not killed_unit->unit_mutables().alive) {
      // we need to check for the card being truly dead, in case it had an
      // e.g. last breath effect, which kept it alive or level up effect (Tryndamere)
      m_state->to_graveyard(killed_unit);
   }
   _remove(killed_unit);
}

void Game::nexus_strike(Player attacked_nexus, const sptr< Unit >& striking_unit)
{
   sptr< long > att_power = std::make_shared< long >(striking_unit->power());
   if(*att_power > 0) {
      _trigger_event(events::NexusStrikeEvent(
         striking_unit->mutables().owner, striking_unit, attacked_nexus, att_power));
      m_state->damage_nexus(*att_power, attacked_nexus);
      m_state->token_plunder(true, Player(1 - attacked_nexus));
   }
}

std::vector< sptr< Card > > Game::_draw_n_cards(Player player, int n)
{
   std::vector< sptr< Card > > hand(n);
   auto& deck = m_state->deck(player);
   choose_inplace(deck, n);
   for(int i = 0; i < n; ++i) {
      hand[i] = deck[i];
   }
   return hand;
}
void Game::_mulligan(
   const std::vector< sptr< Card > >& hand_blue, const std::vector< sptr< Card > >& hand_red)
{
   std::array< std::vector< sptr< Card > >, 2 > hands{hand_blue, hand_red};

   auto mull_act_blue = m_agents[BLUE]->decide_mulligan(*m_state, hand_blue);
   auto mull_act_red = m_agents[RED]->decide_mulligan(*m_state, hand_red);

   std::array< std::array< bool, INITIAL_HAND_SIZE >, 2 > replace = {
      mull_act_blue->get_replace_decisions(), mull_act_red->get_replace_decisions()};

   for(int p = BLUE; p != RED; ++p) {
      auto& curr_hand = hands.at(p);
      State::HandType new_hand;
      auto nr_cards_to_replace = 0;
      auto replace_for_p = replace.at(p);
      auto hand_size = curr_hand.size();
      for(auto i = 0U; i < hand_size; ++i) {
         if(replace_for_p.at(i)) {
            shuffle_card_into_deck(curr_hand.at(i), Player(p));
            nr_cards_to_replace += 1;
         }
      }

      auto new_cards = _draw_n_cards(Player(p), nr_cards_to_replace);
      for(size_t i = 0; i < hand_size; ++i) {
         if(replace_for_p.at(i)) {
            new_hand.at(i) = new_cards.back();
            new_cards.pop_back();
         } else {
            new_hand.at(i) = curr_hand.at(i);
         }
      }
      m_state->hand(new_hand, Player(p));
   }
}
void Game::_end_round()
{
   // first let all effects trigger that state an effect with the "Round End" keyword
   auto active_player = m_state->active_player();
   auto passive_player = Player(1 - active_player);
   _trigger_event(events::RoundEndEvent(active_player, std::make_shared< long >(m_state->round())));

   SymArr< std::vector< sptr< Unit > > > regenerating_units;
   auto end_round_procedure = [&](Player player) {
      // kill ephemeral units
      for(auto& unit : m_state->m_board->get_camp(player)) {
         if(unit->has_keyword(Keyword::EPHEMERAL)) {
            kill_unit(player, to_unit(unit));
         }
         // remove temporary grants
         // undo temporary buffs/nerfs and possibly heal the units if applicable
         auto temp_grants = unit->mutables().grants_temp;
         for(auto&& grant : temp_grants) {
            grant->undo();
         }
         temp_grants.clear();

         // REGENERATION units will regenerate after removing grants etc.
         if(unit->has_keyword(Keyword::REGENERATION)) {
            regenerating_units[player].emplace_back(to_unit(unit));
         }
      }

      // float spell mana if available
      m_state->spell_mana(m_state->spell_mana(player) + m_state->mana(player), player);
   };

   auto regenerate_units = [&](Player player) {
      // regenerate the units with regeneration
      for(auto& unit : regenerating_units[player]) {
         if(unit->unit_mutables().alive) {
            heal(unit->mutables().owner, unit, unit->unit_mutables().damage);
         }
      }
   };
   end_round_procedure(active_player);
   end_round_procedure(passive_player);

   regenerate_units(active_player);
   regenerate_units(passive_player);
}
void Game::_start_round()
{
   m_state->incr_round();

   m_state->token_plunder(false, BLUE);
   m_state->token_plunder(false, RED);
   m_state->reset_pass_all();

   size_t round = m_state->round();
   auto attacker = Player(m_state->starting_player() + round % 2);
   m_state->token_attack(attacker == BLUE, BLUE);
   m_state->token_attack(attacker == RED, RED);
   m_state->token_scout(BLUE, false);
   m_state->token_scout(RED, false);

   // the round start events is to be triggered by the player that also ended the previous round.
   _trigger_event(
      events::RoundStartEvent(m_state->active_player(), std::make_shared< long >(round)));
   for(int player = BLUE; player != RED; ++player) {
      incr_managems(Player(player));
   }
   m_state->refill_mana(BLUE);
   m_state->refill_mana(RED);

   draw_card(BLUE);
   draw_card(RED);
}
void Game::incr_managems(Player player, long amount)
{
   m_state->incr_managems(player, amount);
   _check_enlightenment(player);
}

void Game::_check_enlightenment(Player player)
{
   if(m_state->is_enlightened(player)) {
      _trigger_event(events::EnlightenmentEvent(player));
   }
}
void Game::retreat_to_camp(Player player)
{
   process_camp_queue(player);

   size_t camp_units_count = m_state->m_board->count_units(player, true);
   size_t bf_units_count = m_state->m_board->count_units(player, false);
   // obliterate any battling units which won't find space on the board
   auto& bf = m_state->m_board->get_battlefield(player);
   if(auto nr_units_to_obliterate = bf_units_count + camp_units_count - CAMP_SIZE;
      nr_units_to_obliterate > 0) {
      size_t last_deleted_idx = BATTLEFIELD_SIZE;
      for(size_t i = 0; i < nr_units_to_obliterate; ++i) {
         for(size_t idx = last_deleted_idx - 1; idx > 0; --idx) {
            if(auto opt_unit = bf.at(idx); has_value(opt_unit)) {
               auto unit_to_obliterate = opt_unit;
               obliterate(unit_to_obliterate);
               reset(opt_unit);
               last_deleted_idx = idx;
            }
         }
      }
   }
   // now move back the battlefield units
   size_t curr_idx = camp_units_count - 1;
   auto& camp = m_state->m_board->get_camp(player);
   for(auto& opt_unit : bf) {
      if(has_value(opt_unit)) {
         camp.at(curr_idx) = opt_unit;
         reset(opt_unit);
      }
      curr_idx += 1;
   }
}
void Game::play_spells()
{
   auto& spell_stack = m_state->spell_stack();
   auto& spell_prestack = m_state->spell_prestack();
   for(const auto& spell : spell_prestack) {
      Player player = spell->mutables().owner;
      spell_stack.emplace_back(spell);
      spend_mana(player, spell->mana_cost(), true);
      uncover_card(spell);
      _play_event_triggers(spell, player);
   }
   spell_prestack.clear();
}

void Game::play(const sptr< Card >& card, std::optional< size_t > replaces)
{
   Player player = card->mutables().owner;
   spend_mana(player, card->mana_cost(), false);
   auto& camp = m_state->m_board->get_camp(player);
   uncover_card(card);
   if(replaces.has_value()) {
      auto& card_to_replace = camp.at(replaces.value());
      obliterate(card_to_replace);
      camp.at(replaces.value()) = card;
   } else {
      camp.at(m_state->m_board->count_units(player, true) - 1) = card;
   }
   card->move(Location::CAMP, has_value(replaces) ? replaces.value() : camp.size() - 1 );
   m_event_listener.register_card(card);
   _play_event_triggers(card, player);
}
void Game::_play_event_triggers(const sptr< Card >& card, const Player& player)
{
   if(card->has_effect(events::EventType::DAYBREAK) && check_daybreak(player)) {
      _trigger_event(events::DaybreakEvent(player, card));
   }
   if(card->has_effect(events::EventType::NIGHTFALL) && check_nightfall(player)) {
      _trigger_event(events::NightfallEvent(player, card));
   }
   _trigger_event(events::PlayEvent(player, card));
}
void Game::cast(const sptr< Spell >& spell)
{
   _trigger_event(events::CastEvent(spell->mutables().owner, spell));
}
void Game::process_camp_queue(Player player)
{
   // the logic of LOR for deciding how many units in the queue fit into the
   // camp goes by 2 different modes:
   // 1) default play mode, and
   // 2) battle resolution mode

   // In 1) the card will be obliterated, if there is no space currently on the board left,
   // after having considered all previous units in the queue. Otherwise, it is placed
   // in the camp. When a unit is summoned when attack is declared (e.g. Kalista summoning
   // Rekindler, who in turn summons another Kalista again) and the EXPECTED number of
   // surviving units is less than MAX_NR_CAMP_SLOTS (e.g. because some of the attacking
   // units are ephemeral and expected to die on strike), then that unit would be placed
   // in camp immediately. If contrary to expectation, some units manage to survive (e.g.
   // through "Death's Hand" spell removing ephemeral or the ephemeral was shadow-blocked),
   // then, starting from the end, any unit of the battlefield, which no longer finds space
   // in the camp, is obliterated (this is handled in the method 'retreat_to_camp').
   size_t camp_units_count = m_state->m_board->count_units(player, true);
   size_t bf_units_count = m_state->m_board->count_units(
      player, false, [](const sptr< Card >& unit) {
         return not unit->has_keyword(Keyword::EPHEMERAL);
      });
   auto& queue = m_state->m_board->get_camp_queue(player);
   size_t units_in_queue = queue.size();
   auto& camp = m_state->m_board->get_camp(player);
   if(auto rem_slots = CAMP_SIZE - camp_units_count - bf_units_count; rem_slots > 0) {
      size_t curr_idx = camp_units_count - 1;
      for(size_t counter = 0; counter < std::min(units_in_queue, rem_slots); ++counter) {
         camp.at(curr_idx) = queue.front();
         queue.pop();
         curr_idx += 1;
      }
   }
}
void Game::spend_mana(Player player, size_t cost, bool spell_mana)
{
   long int rem_mana_to_pay = cost;
   if(spell_mana) {
      auto curr_spell_mana = m_state->spell_mana(player);
      rem_mana_to_pay = static_cast< long int >(cost - curr_spell_mana);
      m_state->spell_mana(-rem_mana_to_pay, player);
   }
   if(rem_mana_to_pay > 0) {
      m_state->mana(m_state->mana(player) + rem_mana_to_pay, player);
   }
}

void Game::draw_card(Player player)
{
   auto deck = m_state->deck(player);
   auto card_drawn = deck.back();
   deck.pop_back();
   if(auto hand = m_state->hand(player); hand.size() < HAND_CARDS_LIMIT) {
      hand.emplace_back(card_drawn);
      _trigger_event(events::DrawCardEvent(player, card_drawn));
   } else {
      obliterate(card_drawn);
   }
}
void Game::summon(const sptr< Unit >& unit)
{
   m_state->m_board->add_to_queue(unit);
   Player player = unit->mutables().owner;
   process_camp_queue(player);
   m_event_listener.register_card(unit);
   _trigger_event(events::SummonEvent(player, unit));
}
void Game::summon_to_battlefield(const sptr< Unit >& unit)
{
   Player player = unit->mutables().owner;
   auto bf = m_state->m_board->get_battlefield(player);
   auto curr_unit_count = m_state->m_board->count_units(player, false);
   if(curr_unit_count < BATTLEFIELD_SIZE) {
      auto index_to_place = std::max(static_cast< long >(curr_unit_count - 1), 0L);
      bf.at(index_to_place) = unit;
      m_event_listener.register_card(unit);
      _trigger_event(events::SummonEvent(player, unit));
   } else {
      _remove(unit);
   }
}

void Game::summon_exact_copy(const sptr< Unit >& unit)
{
   Player player = unit->mutables().owner;
   auto copied_unit = std::make_shared< Unit >(*unit);
   m_state->m_board->add_to_queue(unit);
   process_camp_queue(player);
   _copy_grants(unit->all_grants(), copied_unit);
   m_event_listener.register_card(copied_unit);
   _trigger_event(events::SummonEvent(player, unit));
}

std::vector< Target > Game::filter_targets_bf(
   const Filter& filter, std::optional< Player > opt_player)
{
   std::vector< Target > targets;

   auto filter_lambda = [&](Player player) {
      auto bf = m_state->m_board->get_battlefield(player);
      for(size_t i = 0; i < BATTLEFIELD_SIZE; ++i) {
         if(const auto& opt_unit = bf.at(i); has_value(opt_unit) && filter(Target(opt_unit))) {
            targets.emplace_back(Target(opt_unit));
         }
      }
   };
   if(opt_player.has_value()) {
      filter_lambda(opt_player.value());
   } else {
      filter_lambda(BLUE);
      filter_lambda(RED);
   }
   return targets;
}

std::vector< Target > Game::filter_targets_camp(
   const Filter& filter, std::optional< Player > opt_player)
{
   std::vector< Target > targets;

   auto filter_lambda = [&](Player player) {
      auto camp = m_state->m_board->get_camp(player);
      for(const auto& unit : camp) {
         if(filter(Target(unit))) {
            targets.emplace_back(Target(unit));
         }
      }
   };
   if(opt_player.has_value()) {
      filter_lambda(opt_player.value());
   } else {
      filter_lambda(BLUE);
      filter_lambda(RED);
   }
   return targets;
}
std::vector< Target > Game::filter_targets_hand(
   const Filter& filter, std::optional< Player > opt_player)
{
   std::vector< Target > targets;

   auto filter_lambda = [&](Player player) {
      auto hand = m_state->hand(player);
      for(const auto& card : hand) {
         if(filter(Target(card))) {
            targets.emplace_back(Target(card));
         }
      }
   };
   if(opt_player.has_value()) {
      filter_lambda(opt_player.value());
   } else {
      filter_lambda(BLUE);
      filter_lambda(RED);
   }
   return targets;
}

std::vector< Target > Game::filter_targets_deck(
   const Filter& filter, std::optional< Player > opt_player)
{
   std::vector< Target > targets;

   auto filter_lambda = [&](Player player) {
      auto deck = m_state->deck(player);
      for(const auto& card : deck) {
         if(filter(Target(card))) {
            targets.emplace_back(Target(card));
         }
      }
   };
   if(opt_player.has_value()) {
      filter_lambda(opt_player.value());
   } else {
      filter_lambda(BLUE);
      filter_lambda(RED);
   }
   return targets;
}

std::vector< Target > Game::filter_targets_board(
   const Filter& filter, std::optional< Player > opt_player)
{
   std::vector< Target > targets;

   if(opt_player.has_value()) {
      Player& player = opt_player.value();
      targets = targets + filter_targets_bf(filter, player) + filter_targets_camp(filter, player);
   } else {
      targets = targets + filter_targets_bf(filter, BLUE) + filter_targets_camp(filter, BLUE)
                + filter_targets_bf(filter, RED) + filter_targets_camp(filter, RED);
   }
   return targets;
}
std::vector< Target > Game::filter_targets_everywhere(
   const Filter& filter, std::optional< Player > opt_player)
{
   std::vector< Target > targets;

   if(opt_player.has_value()) {
      Player& player = opt_player.value();

      targets = targets + filter_targets_bf(filter, player) + filter_targets_camp(filter, player)
                + filter_targets_hand(filter, player);
   } else {
      targets = targets + filter_targets_bf(filter, BLUE) + filter_targets_camp(filter, BLUE)
                + filter_targets_hand(filter, BLUE) + filter_targets_bf(filter, RED)
                + filter_targets_camp(filter, RED) + filter_targets_hand(filter, RED);
   }
   return targets;
}
void Game::heal(Player player, const sptr< Unit >& unit, long amount)
{
   if(amount > 0) {
      unit->heal(amount);
      _trigger_event(events::HealUnitEvent(player, unit, std::make_shared< long >(amount)));
   }
}
bool Game::check_daybreak(Player player) const
{
   const auto& history = m_state->history()->at(player).at(m_state->round());
   // if no play action is found in the history (actions are committed to history only
   // after having been processed), then daybreak is happening.
   return std::find_if(
             history.begin(),
             history.end(),
             [](const sptr< Action >& action) {
                return action->get_action_type() == ActionType::PLAY;
             })
          == history.end();
}
bool Game::check_nightfall(Player player) const
{
   const auto& history = m_state->history()->at(player).at(m_state->round());
   // if any play action is found in the current history (actions are committed to history only
   // after having been processed), then nightfall is happening.
   return std::find_if(
             history.begin(),
             history.end(),
             [](const sptr< Action >& action) {
                return action->get_action_type() == ActionType::PLAY;
             })
          != history.end();
}
void Game::_remove(const sptr< Card >& card)
{
   m_event_listener.unregister_card(card);
   if(auto loc = card->mutables().location; loc == Location::CAMP) {
      algo::remove_element(m_state->m_board->get_camp(card->mutables().owner), card);
   } else if(loc == Location::BATTLEFIELD) {
      algo::remove_element(
         m_state->m_board->get_battlefield(card->mutables().owner), card);
   } else if(loc == Location::HAND) {
      algo::remove_element(m_state->hand(card->mutables().owner), card);
   }
}
void Game::_copy_grants(
   const std::vector< sptr< Grant > >& grants, const std::shared_ptr< Unit >& unit)
{
}
void Game::_cast_spellstack()
{
   auto& stack = m_state->spell_stack();
   // iterate from the end, since the stack is LIFO
   for(auto& spell : reverse(stack)) {
      if(spell->check_cast_condition(*this)) {
         _trigger_event(events::CastEvent(spell->mutables().owner, spell));
      }
   }
   stack.clear();
}
