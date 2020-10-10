
#include "game.h"

#include <utility>

#include "cards/grant.h"
#include "rulesets.h"
#include "utils.h"

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
         Player turn = m_state->get_turn();
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
   m_state->commit_to_history(action);
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
            auto [player, round, _, card, replaces] = cast_action
                                                         ->get_action_data();
            if(card->get_card_type() == CardType::SPELL) {
               auto spell = std::dynamic_pointer_cast< Spell >(card);
               cast(spell);
               if(! spell->has_keyword(Keyword::BURST)) {
                  return false;
               }
            } else {
               play(to_unit(card), replaces);
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

            m_board->move_to_battlefield(positions_own, player);
            m_board->move_to_battlefield(positions_opp, Player(1 - player));
            if(opt_spells.has_value()) {
               for(const auto& spell : opt_spells.value()) {
                  cast(spell);
               }
            }
            _activate_battlemode(player, positions_own);
            process_camp_queue(player);
         }

         case BLOCK: {
            auto cast_action = std::dynamic_pointer_cast< BlockAction >(action);
            auto
               [player,
                round,
                action_type,
                positions,
                opt_spells] = cast_action->get_action_data();
            m_board->move_to_battlefield(positions, player);

            if(opt_spells.has_value()) {
               for(const auto& spell : opt_spells.value()) {
                  cast(spell);
               }
            }
            _trigger_event(events::BlockEvent(player, positions));
         }

         case ACCEPT: {
            _resolve_spell_stack();
            if(m_battle_mode) {
               _resolve_battle();
               _deactivate_battlemode();
            }
         }

         default: break;
      }
   }
   return true;
}
void Game::_activate_battlemode(
   Player attack_player, std::vector< size_t > positions)
{
   m_battle_mode = true;
   m_state->set_attacker(attack_player);
   _trigger_event(events::AttackEvent(attack_player, std::move(positions)));
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
      m_state->set_flag_can_attack(false, attack_player);
   }
}
void Game::_deactivate_battlemode()
{
   m_battle_mode = false;
   m_state->reset_attacker();
}
void Game::_resolve_spell_stack()
{
   auto& spell_stack = m_state->get_spell_stack();
   while(! spell_stack.empty()) {
      auto last_spell = spell_stack.back();
      spell_stack.pop_back();
      cast(last_spell);
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
               if(unit_att->get_health() < 1) {
                  kill_unit(defender, unit_att, unit_att);
               }
               // check whether to kill defending unit and do overwhelm
               // damage
               if(unit_def->get_health() < 1) {
                  kill_unit(attacker, unit_def, unit_att);

                  if(unit_att->has_keyword(Keyword::OVERWHELM)) {
                     sptr< size_t > overwhelm_dmg = std::make_shared< size_t >(
                        -health_def_after);

                     nexus_strike(defender, overwhelm_dmg, unit_att);
                  }
               }
            }
         } else {
            // if there is no defender to block the attack, the attacker
            // strikes the nexus
            sptr< size_t > att_power = std::make_shared< size_t >(
               unit_att->get_power());

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
long int Game::strike(
   const std::shared_ptr< Unit >& unit_att, std::shared_ptr< Unit >& unit_def)
{
   Player attacker = unit_att->get_owner();
   sptr< size_t > att_power = std::make_shared< size_t >(unit_att->get_power());

   if(*att_power > 0) {
      _trigger_event(events::StrikeEvent(attacker, unit_att));
      return deal_damage_to_unit(unit_att, unit_def, att_power);
   }
   return unit_def->get_health();
}
long int Game::deal_damage_to_unit(
   const sptr< Card >& cause,
   std::shared_ptr< Unit >& unit,
   const sptr< size_t >& damage)
{
   _trigger_event(events::UnitTakeDamageEvent(cause, unit, damage));
   unit->take_damage(*damage);
   return unit->get_health();
}
void Game::kill_unit(
   Player killer,
   const std::shared_ptr< Unit >& killed_unit,
   const std::optional< std::shared_ptr< Card > >& cause)
{
   killed_unit->die(*this);
   _trigger_event(events::DieEvent(killer, killed_unit, cause));
   if(! killed_unit->is_alive()) {
      // we need to check for the card being truly dead, in case it had an
      // e.g. last breath effect, which kept it alive
      m_state->move_to_graveyard(killed_unit);
   }
}

void Game::nexus_strike(
   Player attacked_nexus,
   const sptr< size_t >& damage,
   const std::shared_ptr< Card >& responsible_card)
{
   _trigger_event(events::NexusStrikeEvent(
      attacked_nexus, responsible_card->get_owner(), damage, responsible_card));

   m_state->damage_nexus(*damage, attacked_nexus);
   if(*damage > 0) {
      m_state->set_flag_can_plunder(true, Player(1 - attacked_nexus));
   }
}

std::vector< sptr< Card > > Game::_draw_initial_hand(Player player)
{
   return m_state->draw_n_cards(player, INITIAL_HAND_SIZE);
}
void Game::_mulligan(
   const std::vector< sptr< Card > >& hand_blue,
   const std::vector< sptr< Card > >& hand_red)
{
   std::array< std::vector< sptr< Card > >, 2 > hands{hand_blue, hand_red};

   auto mull_act_blue = m_agents[BLUE]->decide_mulligan(*m_state, hand_blue);
   auto mull_act_red = m_agents[RED]->decide_mulligan(*m_state, hand_red);

   std::array< std::array< bool, INITIAL_HAND_SIZE >, 2 > replace = {
      mull_act_blue->get_replace_decisions(),
      mull_act_red->get_replace_decisions()};

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
   _trigger_event(events::RoundEndEvent(m_state->get_round()));
   for(int p = 0; p != 1; ++p) {
      auto player = Player(p);
      // undo temporary buffs/nerfs and possibly heal the units if applicable
      for(auto&& key_grantvec_pair : m_grants_temp) {
         for(auto&& grant : key_grantvec_pair.second) {
            grant->undo();
         }
      }
      m_grants_temp.clear();
      // float spell mana if available
      auto remaining_mana = m_state->get_mana(player);
      m_state->set_spell_mana(
         m_state->get_spell_mana(player) + remaining_mana, player);
   }
}
void Game::_start_round()
{
   m_state->incr_round();

   m_state->set_flag_can_plunder(false, BLUE);
   m_state->set_flag_can_plunder(false, RED);

   auto attacker = Player(
      m_state->get_starting_player() + m_state->get_round() % 2);
   m_state->set_flag_can_attack(attacker == BLUE, BLUE);
   m_state->set_flag_can_attack(attacker == RED, RED);
   m_state->set_scout_token(BLUE, false);
   m_state->set_scout_token(RED, false);

   _trigger_event(events::RoundStartEvent(m_state->get_round()));
   for(int player = BLUE; player != RED; ++player) {
      incr_managems(Player(player));
   }
   m_state->fill_mana(BLUE);
   m_state->fill_mana(RED);

   draw_card(BLUE);
   draw_card(RED);
}
void Game::incr_managems(Player player, size_t amount)
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
void Game::decr_managems(Player player, size_t amount)
{
   m_state->decr_managems(player, amount);
}
void Game::play(const sptr< Unit >& unit, std::optional< size_t > replaces)
{
   Player player = unit->get_owner();
   spend_mana(player, unit->get_mana_cost(), false);
   auto& camp = m_board->get_camp(player);
   if(replaces.has_value()) {
      auto& unit_to_replace = camp.at(replaces.value());
      obliterate(unit_to_replace);
      camp.at(replaces.value()) = unit;
   } else {
      camp.at(m_board->count_units(player, true) - 1) = unit;
   }
   m_event_listener.register_card(unit);
   _trigger_event(events::PlayEvent(unit));
}
void Game::retreat_to_camp(Player player)
{
   process_camp_queue(player);

   size_t camp_units_count = m_board->count_units(player, true);
   size_t bf_units_count = m_board->count_units(player, false);
   // obliterate any battling units which won't find space on the board
   auto& bf = m_board->get_battlefield(player);
   if(auto nr_units_to_obliterate = bf_units_count + camp_units_count
                                    - CAMP_SIZE;
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
   size_t bf_units_count = m_board->count_units(
      player, false, [](const sptr< Unit >& unit) {
         return ! unit->has_keyword(Keyword::EPHEMERAL);
      });
   auto& queue = m_board->get_camp_queue(player);
   size_t units_in_queue = queue.size();
   auto& camp = m_board->get_camp(player);
   if(auto rem_slots = CAMP_SIZE - camp_units_count - bf_units_count;
      rem_slots > 0) {
      size_t curr_idx = camp_units_count - 1;
      for(size_t counter = 0; counter < std::min(units_in_queue, rem_slots);
          ++counter) {
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
void Game::cast(const sptr< Spell >& spell)
{
   // spells dont react to events particularly, so they are passed a None-
   // event

   (*spell)(*this, events::NoneEvent());
   _trigger_event(events::CastEvent(spell));
}

void Game::play(const sptr< Spell >& spell)
{
   Player player = spell->get_owner();
   for(auto& effect :
       spell->get_effects_map().at(events::NoneEvent::event_type)) {
      effect.choose_targets(*this, player);
   }
   spend_mana(player, spell->get_mana_cost(), true);
   m_state->get_spell_stack().emplace_back(spell);
   _trigger_event(events::PlayEvent(spell));
}

void Game::summon(const sptr< Unit >& unit)
{
   m_board->add_to_queue(unit);
   process_camp_queue(unit->get_owner());
   m_event_listener.register_card(unit);
   _trigger_event(events::SummonEvent(unit));
}
void Game::summon_to_battlefield(const sptr< Unit >& unit)
{
   Player player = unit->get_owner();
   auto bf = m_board->get_battlefield(player);
   auto curr_unit_count = m_board->count_units(player, false);
   if(curr_unit_count < BATTLEFIELD_SIZE) {
      auto index_to_place = std::max(
         static_cast< long >(curr_unit_count - 1), 0L);
      bf.at(index_to_place) = unit;
      m_event_listener.register_card(unit);
      _trigger_event(events::SummonEvent(unit));
   } else {
      obliterate(unit);
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
   _trigger_event(events::SummonEvent(unit));
}
std::vector< sptr< Grant > > Game::get_all_grants(
   const sptr< Card >& card) const
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
std::vector< Target > Game::filter_targets(
   Location range,
   const std::function< bool(const sptr< Card >&) >& filter,
   std::optional< Player > player)
{
   switch(range) {
      case Location::BATTLEFIELD: {
         return filter_targets_bf(filter, player);
      }
      case Location::CAMP: {
         return filter_targets_camp(filter, player);
      }
      case Location::BOARD: {
         return filter_targets_board(filter, player);
      }
      case Location::HAND: {
         return filter_targets_hand(filter, player);
      }
      case Location::DECK: {
         return filter_targets_deck(filter, player);
      }
      case Location::EVERYWHERE: {
         return filter_targets_everywhere(filter, player);
      }
   }
}

std::vector< Target > Game::filter_targets_bf(
   const std::function< bool(const sptr< Unit >&) >& filter,
   std::optional< Player > opt_player)
{
   std::vector< Target > targets;

   auto filter_lambda = [&](Player player) {
      auto bf = m_board->get_battlefield(player);
      for(size_t i = 0; i < BATTLEFIELD_SIZE; ++i) {
         if(const auto& opt_unit = bf.at(i);
            opt_unit.has_value() && filter(opt_unit.value())) {
            targets.emplace_back(Target{
               false, player, Location::BATTLEFIELD, i, opt_unit.value()});
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
   const std::function< bool(const sptr< Unit >&) >& filter,
   std::optional< Player > opt_player)
{
   std::vector< Target > targets;

   auto filter_lambda = [&](Player player) {
      auto camp = m_board->get_camp(player);
      for(size_t i = 0; i < camp.size(); ++i) {
         if(auto unit = camp.at(i); filter(unit)) {
            targets.emplace_back(
               Target{false, player, Location::CAMP, i, unit});
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
   const std::function< bool(const sptr< Card >&) >& filter,
   std::optional< Player > opt_player)
{
   std::vector< Target > targets;

   auto filter_lambda = [&](Player player) {
      auto* hand = m_state->get_hand(player);
      for(size_t i = 0; i < hand->size(); ++i) {
         if(auto card = hand->at(i); filter(card)) {
            targets.emplace_back(
               Target{false, player, Location::HAND, i, card});
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
   const std::function< bool(const sptr< Card >&) >& filter,
   std::optional< Player > opt_player)
{
   std::vector< Target > targets;

   auto filter_lambda = [&](Player player) {
     auto* deck = m_state->get_deck(player);
     for(size_t i = 0; i < deck->size(); ++i) {
        if(auto card = deck->at(i); filter(card)) {
           targets.emplace_back(
              Target{false, player, Location::DECK, i, card});
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
   const std::function< bool(const sptr< Unit >&) >& filter,
   std::optional< Player > opt_player)
{
   std::vector< Target > targets;

   if(opt_player.has_value()) {
      Player& player = opt_player.value();
      targets = targets + filter_targets_bf(filter, player)
                + filter_targets_camp(filter, player);
   } else {
      targets = targets + filter_targets_bf(filter, BLUE)
                + filter_targets_camp(filter, BLUE)
                + filter_targets_bf(filter, RED)
                + filter_targets_camp(filter, RED);
   }
   return targets;
}

std::vector< Target > Game::filter_targets_everywhere(
   const std::function< bool(const sptr< Card >&) >& filter,
   std::optional< Player > opt_player)
{
   std::vector< Target > targets;

   if(opt_player.has_value()) {
      Player& player = opt_player.value();

      targets = targets + filter_targets_bf(filter, player)
                + filter_targets_camp(filter, player)
                + filter_targets_hand(filter, player);
   } else {
      targets = targets + filter_targets_bf(filter, BLUE)
                + filter_targets_camp(filter, BLUE)
                + filter_targets_hand(filter, BLUE)
                + filter_targets_bf(filter, RED)
                + filter_targets_camp(filter, RED)
                + filter_targets_hand(filter, RED);
   }
   return targets;
}
//void Game::level_up_champion(sptr<Champion> champ)
//{
//   auto curr_uuid = champ->get_uuid();
//   auto grants = get_all_grants(champ);
//   auto damage = champ->get_damage();
//
//}