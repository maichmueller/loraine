
#include "game.h"

#include <utility>

#include "cards/grant.h"
#include "rulesets.h"
#include "utils.h"

void Game::_move_units(const std::vector< size_t >& positions, Player player, bool to_bf)
{
   auto min_target_pos = m_board->count_units(player, not to_bf);
   auto& battlefield = m_board->get_battlefield(player);
   auto& camp = m_board->get_camp(player);
   if(to_bf) {
      for(size_t i = 0; i < positions.size(); ++i) {
         size_t source_pos = positions.at(i);
         size_t target_pos = i + min_target_pos;
         auto& unit = camp.at(source_pos);
         battlefield.at(target_pos) = to_unit(unit);
         camp.erase(camp.begin() + source_pos);
      }
   } else {
      for(size_t i = 0; i < positions.size(); ++i) {
         size_t source_pos = positions.at(i);
         size_t target_pos = i + min_target_pos;
         auto& opt_unit = battlefield.at(source_pos);
         camp.at(target_pos) = opt_unit.value();
         opt_unit.reset();
      }
   }
}
void Game::_move_units_opp(const std::map< size_t, size_t >& positions, Player player, bool to_bf)
{
   auto* battlefield = &(m_board->get_battlefield(player));
   decltype(battlefield) battlefield_opp_ptr;

   auto& camp = m_board->get_camp(player);

   std::function< void(size_t, size_t) > switch_lambda;
   if(to_bf) {
      battlefield_opp_ptr = &m_board->get_battlefield(Player(1 - player));
      switch_lambda = [&](size_t source_pos, size_t target_pos) {
         auto& unit = camp.at(source_pos);
         if(not unit->has_keyword(Keyword::VULNERABLE)
            && not battlefield_opp_ptr->at(target_pos).value()->has_keyword(Keyword::CHALLENGER)) {
            throw std::invalid_argument(
               "A unit was dragged which was neither VULNERABLE nor had been challenged by a "
               "CHALLENGER.");
         }
         battlefield->at(target_pos) = to_unit(unit);
         camp.erase(camp.begin() + source_pos);
      };
   } else {
      switch_lambda = [&](size_t source_pos, size_t target_pos) {
         auto& opt_unit = battlefield->at(source_pos);
         camp.at(target_pos) = opt_unit.value();
         opt_unit.reset();
      };
   }

   for(const auto& pos : positions) {
      switch_lambda(pos.first, pos.second);
   }
}

bool Game::run_game()
{
   // decide the player who starts attacking
   Player starting_player = Player(
      std::uniform_int_distribution< size_t >(0, 1)(rng::rng_def_engine));
   m_state->set_starting_player(starting_player);

   // draw the starting cards for each player and let them _mulligan
   auto hand_blue = _draw_initial_hand(BLUE);
   auto hand_red = _draw_initial_hand(RED);
   _mulligan(hand_blue, hand_red);

   // the actual run of the game. We play until the state says it is in a
   // terminal state.

   while(true) {
      // ask current agent for action
      Player turn = m_state->get_turn();
      auto action = m_agents[turn]->decide_action(*m_state);

      if(_do_action(action)) {
         m_state->incr_turn();
      }

      if(auto terminality = m_state->is_terminal() != ONGOING) {
         return terminality;
      }
   }
}

bool Game::_do_action(const sptr< AnyAction >& action)
{
   auto action_type = action->get_action_type();
   bool flip_initiative = true;  // whether the other player gets to act next.
   if(action_type == ActionType::PASS) {
      // this case has to be handled separately to combine the pass counter
      // reset for all other cases
      if(m_state->pass()) {
         _end_round();
         _start_round();
      }

   } else {
      m_state->reset_pass();

      switch(action_type) {
         case PLAY: {
            auto cast_action = std::dynamic_pointer_cast< PlayAction >(action);
            auto card = cast_action->get_card_played();
            if(card->get_card_type() == CardType::SPELL) {
               auto spell = std::dynamic_pointer_cast< Spell >(card);
               play(spell);
               if(spell->has_keyword(Keyword::BURST)) {
                  _resolve_spell_stack(true);
                  flip_initiative = false;
               }
            } else {
               play(to_unit(card), cast_action->get_replace_idx());
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
            auto player = cast_action->get_player();
            auto round = cast_action->get_round();
            auto spell = cast_action->get_spell();
            flip_initiative = false;
         }

         case ATTACK: {
            auto cast_action = std::dynamic_pointer_cast< AttackAction >(action);
            auto player = cast_action->get_player();
            auto opt_spells = cast_action->get_potential_spells();

            if(opt_spells.has_value()) {
               for(const auto& spell : opt_spells.value()) {
                  play(spell);
               }
            }
            _activate_battlemode(player);
            process_camp_queue(player);
         }

         case BLOCK: {
            auto cast_action = std::dynamic_pointer_cast< BlockAction >(action);
            auto player = cast_action->get_player();
            auto opt_spells = cast_action->get_potential_spells();

            if(opt_spells.has_value()) {
               for(const auto& spell : opt_spells.value()) {
                  play(spell);
               }
            }
            _trigger_event(events::BlockEvent(player));
         }

         case ACCEPT: {
            _resolve_spell_stack(false);
            if(m_battle_mode) {
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
   m_battle_mode = true;
   m_state->set_attacker(attack_player);
   auto& bf = m_board->get_battlefield(attack_player);
   auto check_att_effects = [&](const sptr< Unit >& unit) {
      if(unit->has_keyword(Keyword::ATTACK)) {
         _trigger_event(events::AttackEvent(attack_player, unit));
      }
   };
   auto check_supp_effects = [&](const sptr< Unit >& unit, const sptr< Unit >& next_unit) {
      if(unit->has_keyword(Keyword::SUPPORT)) {
         _trigger_event(events::SupportEvent(attack_player, unit, Target(next_unit)));
      }
   };

   for(int i = 0; i < BATTLEFIELD_SIZE - 1; ++i) {
      if(bf[i].has_value()) {
         const auto& attack_unit = bf[i].value();
         check_att_effects(attack_unit);
         if(auto next_unit = bf[i + 1]; next_unit.has_value()) {
            check_supp_effects(attack_unit, next_unit.value());
         }
      }
   }
   if(auto last_att = bf[BATTLEFIELD_SIZE]; last_att.has_value()) {
      check_att_effects(last_att.value());
   }

   // remove the attack token of the attacker, as it is now used up,
   // unless all units were scouts (for the first attack)
   bool scout_attack = true;
   if(! m_state->get_scout_token(attack_player)) {
      for(auto opt_unit : m_board->get_battlefield(attack_player)) {
         if(opt_unit.has_value()) {
            if(! opt_unit.value()->has_keyword(Keyword::SCOUT)) {
               scout_attack = false;
               break;
            }
         }
      }
   } else {
      scout_attack = false;
   }
   if(scout_attack) {
      m_state->set_scout_token(attack_player, true);
      _trigger_event(events::ScoutEvent(attack_player));
   } else {
      m_state->set_flag_attack(false, attack_player);
   }
}
void Game::_deactivate_battlemode()
{
   m_battle_mode = false;
   m_state->reset_attacker();
}
void Game::_resolve_spell_stack(bool burst)
{
   auto& spell_stack = m_state->get_spell_stack();
   if(burst) {
      auto last_spell = spell_stack.back();
      spell_stack.pop_back();
      cast(last_spell);
   } else {
      while(! spell_stack.empty()) {
         auto last_spell = spell_stack.back();
         spell_stack.pop_back();
         cast(last_spell);
      }
   }
}
void Game::_resolve_battle()
{
   auto attacker = m_state->get_attacker().value();
   auto defender = Player(1 - attacker);
   auto& battlefield_att = m_board->get_battlefield(attacker);
   auto& battlefield_def = m_board->get_battlefield(defender);
   for(auto pos = 0U; pos < m_board->count_units(attacker, false); ++pos) {
      auto unit_att_opt = battlefield_att.at(pos);

      if(unit_att_opt.has_value()) {
         // if this spot on the battlefield sees an actual unit posted for
         // attack we need to make it strike
         auto unit_att = std::dynamic_pointer_cast< Unit >(unit_att_opt.value());

         auto unit_def_opt = battlefield_def.at(pos);
         if(unit_def_opt.has_value()) {
            // if there is a defender posted on the same spot of the
            // attacker, it needs to battle the attacker
            auto unit_def = std::dynamic_pointer_cast< Unit >(unit_def_opt.value());

            if(unit_def->is_alive()) {
               bool quick_attacks = unit_att->has_keyword(Keyword::QUICK_ATTACK);
               bool double_attacks = unit_att->has_keyword(Keyword::DOUBLE_ATTACK);

               long health_def_after = 0;
               long attack_power = unit_att->get_power();

               if(quick_attacks || double_attacks) {
                  strike(unit_att, unit_def);
                  if(unit_def->get_health() > 0) {
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
               if(unit_att->get_health() == 0) {
                  kill_unit(defender, unit_att, unit_att);
               }
               // check whether to kill defending unit
               if(unit_def->get_health() == 0) {
                  kill_unit(attacker, unit_def, unit_att);
               }
            }
         } else {
            // if there is no defender to block the attack, the attacker
            // strikes the nexus
            sptr< long > att_power = std::make_shared< long >(unit_att->get_power());

            if(*att_power > 0) {
               nexus_strike(defender, att_power, unit_att);
            }
         }
      }
      retreat_to_camp(attacker);
      retreat_to_camp(defender);
      m_battle_mode = false;
   }
}
void Game::strike(const std::shared_ptr< Unit >& unit_att, std::shared_ptr< Unit >& unit_def)
{
   sptr< long > damage = std::make_shared< long >(unit_att->get_power());
   if(*damage > 0) {
      _trigger_event(events::StrikeEvent(unit_att->get_owner(), unit_att, unit_def));
      deal_damage_to_unit(unit_att, unit_def, damage);
   }
   if(unit_att->has_keyword(Keyword::OVERWHELM)) {
      nexus_strike(unit_def->get_owner(), damage, unit_att);
   }
}
void Game::deal_damage_to_unit(
   const sptr< Card >& cause, const sptr< Unit >& unit, const sptr< long >& damage)
{
   long health_def = unit->get_health();
   _trigger_event(events::UnitTakeDamageEvent(cause->get_owner(), cause, Target(unit), damage));
   unit->take_damage(*damage);
   // store any surplus damage in the damage ptr (e.g. for overwhelm dmg calc)
   *damage += -health_def;
}
void Game::kill_unit(Player killer, const sptr< Unit >& killed_unit, const sptr< Card >& cause)
{
   killed_unit->die();
   _trigger_event(events::DieEvent(killer, Target(killed_unit), cause));
   if(not killed_unit->is_alive()) {
      // we need to check for the card being truly dead, in case it had an
      // e.g. last breath effect, which kept it alive or level up effect (Tryndamere)
      m_state->add_to_graveyard(killed_unit);
   }
   remove(killed_unit);
}

void Game::nexus_strike(
   Player attacked_nexus, const sptr< long >& damage, const sptr< Card >& responsible_card)
{
   if(*damage > 0) {
      _trigger_event(events::NexusStrikeEvent(
         responsible_card->get_owner(), responsible_card, attacked_nexus, damage));

      m_state->damage_nexus(*damage, attacked_nexus);

      m_state->set_flag_plunder(true, Player(1 - attacked_nexus));
   }
}

std::vector< sptr< Card > > Game::_draw_initial_hand(Player player)
{
   return m_state->draw_n_cards(player, INITIAL_HAND_SIZE);
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
            m_state->shuffle_card_into_deck(curr_hand.at(i), Player(p));
            nr_cards_to_replace += 1;
         }
      }
      auto new_cards = m_state->draw_n_cards(Player(p), nr_cards_to_replace);
      for(size_t i = 0; i < hand_size; ++i) {
         if(replace_for_p.at(i)) {
            new_hand.at(i) = new_cards.back();
            new_cards.pop_back();
         } else {
            new_hand.at(i) = curr_hand.at(i);
         }
      }
      m_state->set_hand(new_hand, Player(p));
   }
}
void Game::_end_round()
{
   // first let all effects trigger that state an effect with the "Round End" keyword
   auto active_player = m_state->get_turn();
   auto passive_player = Player(1 - active_player);
   _trigger_event(
      events::RoundEndEvent(active_player, std::make_shared< long >(m_state->get_round())));

   SymArr< std::vector< sptr< Unit > > > regenerators;
   auto end_round_procedure = [&](Player player) {
      // kill ephemeral units
      for(auto& unit : m_board->get_camp(player)) {
         if(unit->has_keyword(Keyword::EPHEMERAL)) {
            kill_unit(player, to_unit(unit));
         }
         // remove temporary grants
         // undo temporary buffs/nerfs and possibly heal the units if applicable
         auto temp_grants = unit->get_grants_temp();
         for(auto&& grant : temp_grants) {
            grant->undo();
         }
         temp_grants.clear();

            // REGENERATION units will regenerate after removing grants etc.
         if(unit->has_keyword(Keyword::REGENERATION)) {
            regenerators[player].emplace_back(to_unit(unit));
         }
      }

      // float spell mana if available
      m_state->set_spell_mana(m_state->get_spell_mana(player) + m_state->get_mana(player), player);
   };

   auto regenerate_units = [&](Player player) {
      // regenerate the units with regeneration
      for(auto& unit : regenerators[player]) {
         if(unit->is_alive()) {
            heal(unit->get_owner(), unit, unit->get_damage());
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

   m_state->set_flag_plunder(false, BLUE);
   m_state->set_flag_plunder(false, RED);
   m_state->reset_pass_all();

   size_t round = m_state->get_round();
   auto attacker = Player(m_state->get_starting_player() + round % 2);
   m_state->set_flag_attack(attacker == BLUE, BLUE);
   m_state->set_flag_attack(attacker == RED, RED);
   m_state->set_scout_token(BLUE, false);
   m_state->set_scout_token(RED, false);

   // the round start event is to be triggered by the player that also ended the previous round.
   _trigger_event(events::RoundStartEvent(m_state->get_turn(), std::make_shared< long >(round)));
   for(int player = BLUE; player != RED; ++player) {
      incr_managems(Player(player));
   }
   m_state->fill_mana(BLUE);
   m_state->fill_mana(RED);

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

   size_t camp_units_count = m_board->count_units(player, true);
   size_t bf_units_count = m_board->count_units(player, false);
   // obliterate any battling units which won't find space on the board
   auto& bf = m_board->get_battlefield(player);
   if(auto nr_units_to_obliterate = bf_units_count + camp_units_count - CAMP_SIZE;
      nr_units_to_obliterate > 0) {
      size_t last_deleted_idx = BATTLEFIELD_SIZE;
      for(size_t i = 0; i < nr_units_to_obliterate; ++i) {
         for(size_t idx = last_deleted_idx - 1; idx > 0; --idx) {
            if(auto opt_unit = bf.at(idx); opt_unit.has_value()) {
               auto unit_to_obliterate = opt_unit.value();
               obliterate(unit_to_obliterate);
               opt_unit.reset();
               last_deleted_idx = idx;
            }
         }
      }
   }
   // now move back the battlefield units
   size_t curr_idx = camp_units_count - 1;
   auto& camp = m_board->get_camp(player);
   for(auto& opt_unit : bf) {
      if(opt_unit.has_value()) {
         camp.at(curr_idx) = opt_unit.value();
         opt_unit.reset();
      }
      curr_idx += 1;
   }
}
void Game::play(const sptr< Unit >& unit, std::optional< size_t > replaces)
{
   Player player = unit->get_owner();
   spend_mana(player, unit->get_mana_cost(), false);
   auto& camp = m_board->get_camp(player);
   uncover_card(unit);
   if(replaces.has_value()) {
      auto& unit_to_replace = camp.at(replaces.value());
      remove(unit_to_replace);
      camp.at(replaces.value()) = unit;
   } else {
      camp.at(m_board->count_units(player, true) - 1) = unit;
   }
   m_event_listener.register_card(unit);
   play_event_triggers(unit, player);
}
void Game::play(const sptr< Spell >& spell)
{
   Player player = spell->get_owner();
   for(auto& effect : spell->get_effects_map().at(events::EventType::CAST)) {
      effect.choose_targets(*m_state, *(get_agent(m_state->get_turn())), player);
   }
   spend_mana(player, spell->get_mana_cost(), true);
   m_state->get_spell_stack().emplace_back(spell);
   play_event_triggers(spell, player);
}
void Game::play_event_triggers(const sptr< Card >& card, const Player& player)
{
   _trigger_event(events::PlayEvent(player, card));
   if(check_daybreak(player)) {
      _trigger_event(events::DaybreakEvent(player, card));
   }
   if(check_nightfall(player)) {
      _trigger_event(events::NightfallEvent(player, card));
   }
}
void Game::cast(const sptr< Spell >& spell)
{
   _trigger_event(events::CastEvent(spell->get_owner(), spell));
}
void Game::process_camp_queue(Player player)
{
   // the logic of LOR for deciding how many units in the queue fit into the
   // camp goes by whether there is space left after counting all units in the
   // camp and currently on the battlefield, except for those battling units
   // that are ephemeral and thus expected to die after striking. If they,
   // contrary to expectation, manage to survive (e.g. through "Death's Hand"
   // spell), then any unit of the battlefield as counted from the end, which is
   // no longer fitting into the camp, is obliterated (this is handled in
   // the method 'retreat_to_camp').
   size_t camp_units_count = m_board->count_units(player, true);
   size_t bf_units_count = m_board->count_units(player, false, [](const sptr< Unit >& unit) {
      return ! unit->has_keyword(Keyword::EPHEMERAL);
   });
   auto& queue = m_board->get_camp_queue(player);
   size_t units_in_queue = queue.size();
   auto& camp = m_board->get_camp(player);
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
      auto curr_spell_mana = m_state->get_spell_mana(player);
      rem_mana_to_pay = static_cast< long int >(cost - curr_spell_mana);
      m_state->set_spell_mana(-rem_mana_to_pay, player);
   }
   if(rem_mana_to_pay > 0) {
      m_state->set_mana(m_state->get_mana(player) + rem_mana_to_pay, player);
   }
}
void Game::draw_card(Player player)
{
   auto* deck = m_state->get_deck(player);
   auto card_drawn = deck->draw_card();
   if(auto* hand = m_state->get_hand(player); hand->size() < HAND_CARDS_LIMIT) {
      hand->emplace_back(card_drawn);
      _trigger_event(events::DrawCardEvent(player, card_drawn));
   } else {
      obliterate(card_drawn);
   }
}

void Game::summon(const sptr< Unit >& unit)
{
   m_board->add_to_queue(unit);
   Player player = unit->get_owner();
   process_camp_queue(player);
   m_event_listener.register_card(unit);
   _trigger_event(events::SummonEvent(player, unit));
}
void Game::summon_to_battlefield(const sptr< Unit >& unit)
{
   Player player = unit->get_owner();
   auto bf = m_board->get_battlefield(player);
   auto curr_unit_count = m_board->count_units(player, false);
   if(curr_unit_count < BATTLEFIELD_SIZE) {
      auto index_to_place = std::max(static_cast< long >(curr_unit_count - 1), 0L);
      bf.at(index_to_place) = unit;
      m_event_listener.register_card(unit);
      _trigger_event(events::SummonEvent(player, unit));
   } else {
      remove(unit);
   }
}
void Game::summon_exact_copy(const sptr< Unit >& unit)
{
   Player player = unit->get_owner();
   auto copied_unit = std::make_shared< Unit >(*unit);
   m_board->add_to_queue(unit);
   process_camp_queue(player);
   copy_grant(get_all_grants(unit), copied_unit);
   m_event_listener.register_card(copied_unit);
   _trigger_event(events::SummonEvent(player, unit));
}
std::vector< sptr< Grant > > Game::get_all_grants(const sptr< Card >& card) const
{
   auto uuid = card->get_uuid();
   std::vector< sptr< Grant > > grants;
   for(const auto& grant : m_grants_temp.at(uuid)) {
      grants.emplace_back(grant);
   }
   for(const auto& grant : m_grants_perm.at(uuid)) {
      grants.emplace_back(grant);
   }
   return grants;
}

std::vector< Target > Game::filter_targets_bf(
   const std::function< bool(const sptr< Unit >&) >& filter, std::optional< Player > opt_player)
{
   std::vector< Target > targets;

   auto filter_lambda = [&](Player player) {
      auto bf = m_board->get_battlefield(player);
      for(size_t i = 0; i < BATTLEFIELD_SIZE; ++i) {
         if(const auto& opt_unit = bf.at(i); opt_unit.has_value() && filter(opt_unit.value())) {
            targets.emplace_back(Target(opt_unit.value()));
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
   const std::function< bool(const sptr< Unit >&) >& filter, std::optional< Player > opt_player)
{
   std::vector< Target > targets;

   auto filter_lambda = [&](Player player) {
      auto camp = m_board->get_camp(player);
      for(size_t i = 0; i < camp.size(); ++i) {
         if(auto unit = camp.at(i); filter(to_unit(unit))) {
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
   const std::function< bool(const sptr< Card >&) >& filter, std::optional< Player > opt_player)
{
   std::vector< Target > targets;

   auto filter_lambda = [&](Player player) {
      auto* hand = m_state->get_hand(player);
      for(size_t i = 0; i < hand->size(); ++i) {
         if(auto card = hand->at(i); filter(card)) {
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
   const std::function< bool(const sptr< Card >&) >& filter, std::optional< Player > opt_player)
{
   std::vector< Target > targets;

   auto filter_lambda = [&](Player player) {
      auto* deck = m_state->get_deck(player);
      for(size_t i = 0; i < deck->size(); ++i) {
         if(auto card = deck->at(i); filter(card)) {
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
   const std::function< bool(const sptr< Unit >&) >& filter, std::optional< Player > opt_player)
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
   const std::function< bool(const sptr< Card >&) >& filter, std::optional< Player > opt_player)
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
   const auto& history = m_state->get_history()->at(player).at(m_state->get_round());
   // if no play action is found in the history (actions are committed to history only
   // after having been processed), then daybreak is happening.
   return std::find_if(
             history.begin(),
             history.end(),
             [](const sptr< AnyAction >& action) {
                return action->get_action_type() == ActionType::PLAY;
             })
          == history.end();
}
bool Game::check_nightfall(Player player) const
{
   const auto& history = m_state->get_history()->at(player).at(m_state->get_round());
   // if any play action is found in the current history (actions are committed to history only
   // after having been processed), then nightfall is happening.
   return std::find_if(
             history.begin(),
             history.end(),
             [](const sptr< AnyAction >& action) {
                return action->get_action_type() == ActionType::PLAY;
             })
          != history.end();
}
void Game::remove(const sptr< Card >& card)
{
   m_event_listener.unregister_card(card);
   auto uuid = card->get_uuid();
   m_grants_perm.erase(uuid);
   m_grants_temp.erase(uuid);
   if(card->is_unit() || card->is_landmark()) {
      m_board->remove_units({to_unit(card)});
   }
}

// void Game::level_up_champion(sptr<Champion> champ)
//{
//   auto curr_uuid = champ->get_uuid();
//   auto grants = get_all_grants(champ);
//   auto damage = champ->get_damage();
//
//}