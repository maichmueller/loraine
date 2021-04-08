#include "core/logic.h"

#include "core/action.h"
#include "core/gamestate.h"

Logic::Logic(const Logic& other)
    : m_state(other.m_state),
      m_action_invoker(other.m_action_invoker->clone()),
      m_prev_action_invoker(other.m_prev_action_invoker->clone())
{
}

void Logic::request_action() const
{
   m_state->action_buffer()->emplace_back(
      std::make_shared< actions::Action >(std::move(m_action_invoker->request_action(*state()))));
}

void Logic::cast(bool burst)
{
   auto* spell_stack = m_state->spell_stack();
   while(not spell_stack->empty()) {
      auto spell = spell_stack->back();
      spell_stack->pop_back();
      trigger_event< events::EventLabel::CAST >(spell->mutables().owner, spell);
      if(burst) {
         break;
      }
   }
}
Status Logic::step()
{
   _start_round();
   bool flip_initiative = false;
   m_state->player(m_state->active_team()).flags()->has_played = false;
   while(not flip_initiative) {
      request_action();
      flip_initiative = invoke_actions();
   }
   if(m_state->player(Team::BLUE).flags()->pass && m_state->player(Team::RED).flags()->pass) {
      _end_round();
   }
   m_state->turn() += 1;
   return check_status();
}
void Logic::play_event_triggers(const sptr< Card >& card)
{
   trigger_event< events::EventLabel::PLAY >(card->mutables().owner, card);
   _trigger_daybreak_if(card);
   _trigger_nightfall_if(card);
}
void Logic::place_in_camp(const sptr< FieldCard >& card, const std::optional< size_t >& replaces)
{
   auto& camp = *m_state->board()->camp(card->mutables().owner);
   if(utils::has_value(replaces)) {
      size_t replace_idx = replaces.value();
      obliterate(camp[replace_idx]);
      camp[replace_idx] = card;
   } else {
      camp.emplace_back(card);
   }
}
void Logic::_trigger_daybreak_if(const sptr< Card >& card)
{
   Team team = card->mutables().owner;
   auto daybreak = m_state->player(team).flags()->is_daybreak;  // copy before overwriting
   m_state->player(team).flags()->is_daybreak = false;  // if there was daybreak, there is none now
   if(daybreak && card->has_effect(events::EventLabel::DAYBREAK)) {
      trigger_event< events::EventLabel::DAYBREAK >(team, card);
   }
}
void Logic::_trigger_nightfall_if(const sptr< Card >& card)
{
   Team team = card->mutables().owner;
   auto nightfall = m_state->player(team).flags()->is_nightfall;  // copy before overwriting
   m_state->player(team).flags()->is_nightfall = true;  // from now on there is certainly nightfall
   if(nightfall && card->has_effect(events::EventLabel::NIGHTFALL)) {
      trigger_event< events::EventLabel::NIGHTFALL >(team, card);
   }
}

void Logic::_start_round()
{
   auto& round = m_state->round();
   round += 1;
   for(Team team : {Team::RED, Team::BLUE}) {
      auto& flags = *m_state->player(team).flags();
      flags.plunder_token = false;
      flags.is_daybreak = true;
      flags.is_nightfall = false;
      flags.attack_token = false;
      flags.scout_token = false;
      reset_pass(team);
      auto& mana = *m_state->player(team).mana();
      give_managems(Team(team));
      refill_mana(BLUE, false);
   }

   Team active_team = Team(m_state->starting_team() + round % 2);
   m_state->attacker(active_team);

   trigger_event< events::EventLabel::ROUND_START >(m_state->active_team(), round);

   draw_card(BLUE);
   draw_card(RED);
}
void Logic::refill_mana(Team team, bool normal_mana)
{
   auto& mana = *m_state->player(team).mana();
   if(normal_mana) {
      mana.common = mana.gems;
   } else {
      mana.common = m_state->config().MAX_FLOATING_MANA;
   }
}

void Logic::draw_card(Team team)
{
   auto* deck = m_state->player(team).deck();
   if(deck->empty()) {
      _set_status(Status::win(opponent(team), false));
      return;
   }
   auto card_drawn = deck->pop();

   trigger_event< events::EventLabel::DRAW_CARD >(team, card_drawn);
   if(auto* hand = m_state->player(team).hand();
      hand->size() < m_state->config().HAND_CARDS_LIMIT) {
      hand->emplace_back(card_drawn);
   } else {
      obliterate(card_drawn);
   }
}

void Logic::give_managems(Team team, long amount)
{
   m_state->player(team).mana()->gems += amount;
   trigger_event< events::EventLabel::GAIN_MANAGEM >(team, amount);
   _check_enlightenment(team);
}
void Logic::_check_enlightenment(Team team)
{
   auto& player = m_state->player(team);
   if(not player.flags()->enlightened
      && player.mana()->gems >= m_state->config().ENLIGHTENMENT_THRESHOLD) {
      player.flags()->enlightened = true;
      trigger_event< events::EventLabel::ENLIGHTENMENT >(team);
   }
}

void Logic::spend_mana(const sptr< Card >& card)
{
   long common_mana_cost = card->mana_cost();
   auto* mana_resource = m_state->player(card->mutables().owner).mana();

   if(card->is_spell()) {
      common_mana_cost = static_cast< size_t >(card->mana_cost() - mana_resource->floating);
      // we have either spent all spell mana and thus reach 0, or there will be leftovers, which
      // will see `-common_mana_cost` > 0
      mana_resource->floating = std::max(0L, -common_mana_cost);
   }
   if(common_mana_cost > 0) {
      // there is some mana left to pay
      if(common_mana_cost > mana_resource->common) {
         throw std::logic_error(
            "The played spell " + card->immutables().name + "costs more than the total mana of the player. An invalid action "
            "wasn't recognized.");
      }
      mana_resource->common -= common_mana_cost;
   }
}

// void Logic::retreat_to_camp(Team team)
//{
//   process_camp_queue(team);
//
//   size_t camp_units_count = m_state->board()->count_units(team, true);
//   size_t bf_units_count = m_state->board()->count_units(team, false);
//   // obliterate any battling units which won't find space on the board
//   auto& bf = m_state->board()->battlefield(team);
//   if(auto nr_units_to_obliterate = bf_units_count + camp_units_count - CAMP_SIZE;
//      nr_units_to_obliterate > 0) {
//      size_t last_deleted_idx = BATTLEFIELD_SIZE;
//      for(size_t i = 0; i < nr_units_to_obliterate; ++i) {
//         for(size_t idx = last_deleted_idx - 1; idx > 0; --idx) {
//            if(auto opt_unit = bf.at(idx); has_value(opt_unit)) {
//               auto unit_to_obliterate = opt_unit;
//               obliterate(unit_to_obliterate);
//               reset(opt_unit);
//               last_deleted_idx = idx;
//            }
//         }
//      }
//   }
//   // now move back the battlefield units
//   size_t curr_idx = camp_units_count - 1;
//   auto& camp = m_state->board()->camp(team);
//   for(auto& opt_unit : bf) {
//      if(has_value(opt_unit)) {
//         camp.at(curr_idx) = opt_unit;
//         reset(opt_unit);
//      }
//      curr_idx += 1;
//   }
//}

// void Logic::process_camp_queue(Team team)
//{
//   // the logic of LOR for deciding how many units in the m_queue fit into the
//   // camp goes by 2 different modes:
//   // 1) default play_event_triggers mode, and
//   // 2) battle resolution mode
//
//   // In 1) the spell will be obliterated, if there is no space currently on the board left,
//   // after having considered all previous units in the m_queue. Otherwise, it is placed
//   // in the camp. When a unit is summoned when attack is declared (e.g. Kalista summoning
//   // Rekindler, who in turn summons another Kalista again) and the EXPECTED number of
//   // surviving units is less than MAX_NR_CAMP_SLOTS (e.g. because some of the attacking
//   // units are ephemeral and expected to die on strike), then that unit would be placed
//   // in camp immediately. If contrary to expectation, some units manage to survive (e.g.
//   // through "Death's Hand" floating removing ephemeral or the ephemeral was shadow-blocked),
//   // then, starting from the end, any unit of the battlefield, which no longer finds space
//   // in the camp, is obliterated (this is handled in the method 'retreat_to_camp').
//   size_t camp_units_count = m_state->board()->count_units(team, true);
//   size_t bf_units_count = m_state->board()->count_units(team, false, [](const sptr< Card >& unit)
//   {
//      return not unit->has_keyword(Keyword::EPHEMERAL);
//   });
//   auto& queue = m_state->board()->camp_queue(team);
//   size_t units_in_queue = queue.size();
//   auto& camp = m_state->board()->camp(team);
//   if(auto rem_slots = CAMP_SIZE - camp_units_count - bf_units_count; rem_slots > 0) {
//      size_t curr_idx = camp_units_count - 1;
//      for(size_t counter = 0; counter < std::min(units_in_queue, rem_slots); ++counter) {
//         camp.at(curr_idx) = queue.front();
//         queue.pop();
//         curr_idx += 1;
//      }
//   }
//}

//

//
Status Logic::check_status()
{
   const auto& cfg = m_state->config();
   if(m_state->round() > cfg.MAX_ROUNDS) {
      m_state->m_status = Status::TIE;
   }
   auto& player_blue = m_state->player(Team::BLUE);
   auto& player_red = m_state->player(Team::RED);
   if(player_blue.nexus()->health() < 1) {
      if(player_red.nexus()->health() < 1) {
         _set_status(Status::TIE);
      }
      _set_status(Status::RED_WINS_NEXUS);
   } else if(player_red.nexus()->health() < 1) {
      _set_status(Status::BLUE_WINS_NEXUS);
   }
   _set_status(Status::ONGOING);
   m_state->m_status_checked = true;
   return m_state->m_status;
}

void Logic::pass()
{
   m_state->player(m_state->active_team()).flags()->pass = true;
}
void Logic::reset_pass(Team team)
{
   m_state->player(team).flags()->pass = false;
}

void Logic::reset_pass()
{
   m_state->player(m_state->active_team()).flags()->pass = false;
}
void Logic::_set_status(Status status)
{
   if(m_state->m_status == Status::ONGOING) {
      // once the status is set to anything but ongoing, it is frozen
      m_state->m_status = status;
   }
}
bool Logic::invoke_actions()
{
   auto* action_buffer = m_state->action_buffer();
   bool flip_initiative = true;
   while(not action_buffer->empty()) {
      auto& action = action_buffer->back();
      m_state->commit_to_history(std::make_unique< ActionRecord >(action));
      flip_initiative = m_action_invoker->invoke(*action);
      action_buffer->pop_back();
   }
   return flip_initiative;
}

// void Logic::_activate_battlemode(Team attack_team)
//{

//
//   // remove the attack token of the attacker, as it is now used up,
//   // unless all units were scouts (for the first attack)
//   bool scout_attack = true;
//   if(! m_state->token_scout(attack_team)) {
//      for(auto opt_unit : m_state->board()->battlefield(attack_team)) {
//         if(has_value(opt_unit)) {
//            if(not opt_unit->has_keyword(Keyword::SCOUT)) {
//               scout_attack = false;
//               break;
//            }
//         }
//      }
//   } else {
//      scout_attack = false;
//   }
//   if(scout_attack) {
//      m_state->token_scout(attack_team, true);
//      _trigger_event(m_subscribed_events::ScoutEvent(attack_team));
//   } else {
//      m_state->token_attack(false, attack_team);
//   }
//}
// void Logic::_deactivate_battlemode()
//{
//   m_state->set_battle_mode(false);
//   m_state->reset_attacker();
//}
// void Logic::cast(bool burst)
//{
//   auto& spell_stack = m_state->spell_stack();
//   if(burst) {
//      auto last_spell = spell_stack.back();
//      spell_stack.pop_back();
//      cast(last_spell);
//   } else {
//      _cast_spellstack();
//      if(m_state->in_battle_mode()) {
//         resolve();
//      }
//   }
//}
void Logic::resolve()
{
   // cast all the spells on the spell stack first
   cast(false);
   // then process the combat if necessary
   if(in_combat()) {
      auto attacker = m_state->attacker().value();
      auto defender = Team(1 - attacker);
      auto& bf_att = *m_state->board()->battlefield(attacker);
      auto& bf_def = *m_state->board()->battlefield(defender);
      for(auto pos = 0U; pos < m_state->board()->max_size_bf(); ++pos) {
         if(pos > bf_att.size()) {
            // we do this dynamic check incase a strike or another effect might have summoned
            // another attacker
            break;
         }
         auto unit_att = bf_att.at(pos);

         auto unit_def = bf_def.at(pos);
         if(utils::has_value(unit_def)) {
            if(unit_def->unit_mutables().alive) {
               bool quick_attacks = unit_att->has_keyword(Keyword::QUICK_ATTACK);
               bool double_attacks = unit_att->has_keyword(Keyword::DOUBLE_ATTACK);

               long health_def_after = 0;
               long attack_power = unit_att->power();

               if(quick_attacks || double_attacks) {
                  strike(unit_att, unit_def, false);
                  if(unit_def->unit_mutables().alive) {
                     if(double_attacks) {
                        // a double attacking unit attacks again after a quick
                        // attack
                        strike(unit_att, unit_def, false);
                     }
                     strike(unit_def, unit_att, false);
                  }

               } else {
                  strike(unit_att, unit_def, false);
                  strike(unit_def, unit_att, false);
               }

               // check whether to kill the attacking unit
               if(unit_att->health() == 0) {
                  kill_unit(unit_att, unit_att);
               }
               // check whether to kill defending unit
               if(unit_def->health() == 0) {
                  kill_unit(unit_def, unit_att);
               }
            }
         } else {
            // if there is no defender to block the attack, the attacker
            // strikes the nexus
            nexus_strike(unit_att, 0);
         }

         retreat_to_camp(attacker);
         retreat_to_camp(defender);
      }
   }
   transition_action_invoker< DefaultModeInvoker >();
}
void Logic::strike(const sptr< Unit >& unit_att, sptr< Unit >& unit_def, bool combat_strike)
{
   auto dmg = unit_att->power();
   long surplus_dmg = 0;
   if(dmg > 0) {
      trigger_event< events::EventLabel::STRIKE >(unit_att->mutables().owner, unit_att, unit_def);
      surplus_dmg = damage_unit(unit_att, unit_def, dmg);
   }
   if(surplus_dmg > 0 && combat_strike && unit_att->has_keyword(Keyword::OVERWHELM)
      && m_state->attacker() == unit_att->mutables().owner) {
      nexus_strike(unit_att, surplus_dmg);
   }
}

// void Logic::strike_mutually(const sptr< Unit >& unit1, sptr< Unit >& unit2)
//{
//   if(unit_att->power() > 0) {
//      trigger_event< events::EventLabel::STRIKE >(unit_att->mutables().owner, unit_att, unit_def);
//      damage_unit(unit_att, unit_def, damage);
//   }
//   if(m_state->in_battle_mode() && m_state->attacker() == unit_att->mutables().owner
//      && unit_att->has_keyword(Keyword::OVERWHELM)) {
//      nexus_strike(unit_def->mutables().owner, 0);
//   }
//}
void Logic::init_attack(Team team)
{
   m_state->player(team).flags()->attack_token = false;
   trigger_event< events::EventLabel::ATTACK >(team);

   auto& attacker_bf = *m_state->board()->battlefield(team);
   // Check for support effects taking place:
   // we need to loop over all potential indices, since attack and support effects might
   // summon further attackers onto the battlefield, which changes the battlefield container
   // sizes (and invalidates existing iterators)
   for(int i = 0; i < m_state->board()->max_size_bf(); ++i) {
      if(not (i < attacker_bf.size() - 1)) {
         // once we reach the last unit on board, we dont check for support effects anymore
         break;
      }
      if(attacker_bf[i]->has_keyword(Keyword::SUPPORT)) {
         trigger_event< events::EventLabel::SUPPORT >(team, attacker_bf[i], attacker_bf[i + 1]);
      }
   }
   transition_action_invoker< CombatModeInvoker >();
}
void Logic::init_block(Team team)
{
   trigger_event< events::EventLabel::BLOCK >(team);
}
long Logic::damage_unit(const sptr< Card >& cause, const sptr< Unit >& unit, long dmg)
{
   long dmg_taken = unit->take_damage(cause, dmg);
   trigger_event< events::EventLabel::UNIT_DAMAGE >(
      cause->mutables().owner, cause, unit, dmg_taken);
   return unit->health();
}
void Logic::kill_unit(const sptr< Unit >& killed_unit, const sptr< Card >& cause)
{
   killed_unit->kill(cause);
   trigger_event< events::EventLabel::SLAY >(cause->mutables().owner, cause, killed_unit);
   if(not killed_unit->unit_mutables().alive) {
      // we need to check for the spell being truly dead, in case it had an
      // e.g. last breath effect, which kept it alive or level up effect (Tryndamere) etc.
      m_state->to_graveyard(killed_unit);
   }
   _remove(killed_unit);
}
void Logic::_remove(const sptr< Card >& card)
{
   Team team = card->mutables().owner;
   if(card->is_fieldcard()) {
      auto loc = card->mutables().location;
      if(loc == Location::CAMP) {
         auto camp = m_state->board()->camp(team);
         camp->erase(std::next(camp->begin(), card->mutables().position));
      }
   }
   unsubscribe_effects(card);
}
void Logic::unsubscribe_effects(const sptr< Card >& card)
{
   unsubscribe_effects_impl< events::n_events - 1 >(card);
}
void Logic::subscribe_effects(
   const sptr< Card >& card,
   EffectBase::RegistrationTime registration_time)
{
   subscribe_effects_impl< events::n_events - 1 >(card, registration_time);
}
void Logic::nexus_strike(const sptr< Unit >& striking_unit, long dmg)
{
   if(dmg > 0) {
      m_state->player(striking_unit->mutables().owner).nexus()->add_health(striking_unit, dmg);
   }
}
//
// void Logic::nexus_strike(const sptr< Unit >& striking_unit)
//{
//   Team attacked_nexus = opponent(striking_unit->mutables().owner);
//   sptr< long > att_power = std::make_shared< long >(striking_unit->power());
//   if(*att_power > 0) {
//      _trigger_event(m_subscribed_events::NexusStrikeEvent(
//         striking_unit->mutables().owner, striking_unit, attacked_nexus, att_power));
//      m_state->damage_nexus(*att_power, attacked_nexus);
//      m_state->token_plunder(true, Team(1 - attacked_nexus));
//   }
//}
//
// std::vector< sptr< Card > > Logic::_draw_n_cards(Team team, int n)
//{
//   std::vector< sptr< Card > > hand(n);
//   auto& deck = m_state->deck(team);
//   choose_inplace(deck, n);
//   for(int i = 0; i < n; ++i) {
//      hand[i] = deck[i];
//   }
//   return hand;
//}
// void Logic::_mulligan(
//   const std::vector< sptr< Card > >& hand_blue,
//   const std::vector< sptr< Card > >& hand_red)
//{
//   std::array< std::vector< sptr< Card > >, 2 > hands{hand_blue, hand_red};
//
//   auto mull_act_blue = m_agents[BLUE]->choose_mulligan(*m_state, hand_blue);
//   auto mull_act_red = m_agents[RED]->choose_mulligan(*m_state, hand_red);
//
//   std::array< std::array< bool, INITIAL_HAND_SIZE >, 2 > m_replace = {
//      mull_act_blue->replace_decisions(), mull_act_red->replace_decisions()};
//
//   for(int p = BLUE; p != RED; ++p) {
//      auto& curr_hand = hands.at(p);
//      GameState::HandType new_hand;
//      auto nr_cards_to_replace = 0;
//      auto replace_for_p = m_replace.at(p);
//      auto hand_size = curr_hand.size();
//      for(auto i = 0U; i < hand_size; ++i) {
//         if(replace_for_p.at(i)) {
//            shuffle_card_into_deck(curr_hand.at(i), Team(p), 0);
//            nr_cards_to_replace += 1;
//         }
//      }
//
//      auto new_cards = _draw_n_cards(Team(p), nr_cards_to_replace);
//      for(size_t i = 0; i < hand_size; ++i) {
//         if(replace_for_p.at(i)) {
//            new_hand.at(i) = new_cards.back();
//            new_cards.pop_back();
//         } else {
//            new_hand.at(i) = curr_hand.at(i);
//         }
//      }
//      m_state->hand(new_hand, Team(p));
//   }
//}
void Logic::_end_round()
{
   // first let all m_effects trigger that state an effect with the "Round End" keyword
   auto active_team = m_state->active_team();
   auto passive_team = opponent(active_team);
   trigger_event< events::EventLabel::ROUND_END >(active_team, m_state->round());

   SymArr< std::vector< sptr< Unit > > > regenerating_units;
   auto end_round_proc = [&](Team team) {
      // kill ephemeral units
      for(auto& unit : *m_state->board()->camp(team)) {
         if(unit->has_keyword(Keyword::EPHEMERAL)) {
            kill_unit(to_unit(unit), unit);
            continue;
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
            regenerating_units[team].emplace_back(to_unit(unit));
         }
      }
      // store floating mana if available
      auto* mana = m_state->player(team).mana();
      mana->floating += mana->common;
   };

   auto regen_proc = [&](Team team) {
      // regenerate the units with regeneration
      for(auto& unit : regenerating_units[team]) {
         if(unit->unit_mutables().alive) {
            heal(to_unit(unit), unit, unit->unit_mutables().damage);
         }
      }
   };

   for(const auto& procedure :
       std::initializer_list< std::function< void(Team) > >{end_round_proc, regen_proc}) {
      procedure(active_team);
      procedure(passive_team);
   }
}
void Logic::heal(const sptr< Unit >& unit, const sptr< Card >& cause, size_t amount)
{
   auto true_amount = unit->heal(amount);
   trigger_event< events::EventLabel::HEAL_UNIT >(
      cause->mutables().owner, unit, cause, true_amount);
}
void Logic::retreat_to_camp(Team team)
{
   auto& bf = *m_state->board()->battlefield(team);
   auto& camp = *m_state->board()->camp(team);
   std::decay_t< decltype(bf) > bf_stack_buffer;
   bf_stack_buffer.reserve(bf.size());
   // we reverse the bf container, because popping from the end of a vector is performant, while
   // popping from the beginning is not, and we need to loop over the bf from the beginning, since
   // the units return to camp from left to right
   for(const auto& unit : utils::reverse(bf)) {
      bf.emplace_back(unit);
   }
   bf.clear();

   while(not bf_stack_buffer.empty()) {
      auto unit = bf_stack_buffer.back();
      bf_stack_buffer.pop_back();
      if(camp.size() >= m_state->config().CAMP_SIZE) {
         // no space left in camp (due to meddling during combat resolution, e.g. removal of an
         // ephemeral keyword), so the unit is obliterated
         obliterate(unit);
      } else {
         camp.emplace_back(unit);
      }
   }
}

// void Logic::summon(const sptr< Unit >& unit, bool to_bf)
//{
//   m_state->board()->add_to_camp_queue(unit);
//   Team team = unit->mutables().owner;
//   process_camp_queue(team);
//   m_event_listener.register_card(unit);
//   _trigger_event(m_subscribed_events::SummonEvent(team, unit));
//}
// void Logic::summon_to_battlefield(const sptr< Unit >& unit)
//{
//   Team team = unit->mutables().owner;
//   auto bf = m_state->board()->battlefield(team);
//   auto curr_unit_count = m_state->board()->count_units(team, false);
//   if(curr_unit_count < BATTLEFIELD_SIZE) {
//      auto index_to_place = std::max(static_cast< long >(curr_unit_count - 1), 0L);
//      bf.at(index_to_place) = unit;
//      m_event_listener.register_card(unit);
//      _trigger_event(m_subscribed_events::SummonEvent(team, unit));
//   } else {
//      _remove(unit);
//   }
//}
//
// void Logic::summon_exact_copy(const sptr< Unit >& unit)
//{
//   Team team = unit->mutables().owner;
//   auto copied_unit = std::make_shared< Unit >(*unit);
//   m_state->board()->add_to_camp_queue(unit);
//   process_camp_queue(team);
//   _copy_grants(unit->all_grants(), copied_unit);
//   m_event_listener.register_card(copied_unit);
//   _trigger_event(m_subscribed_events::SummonEvent(team, unit));
//}
//
// std::vector< Target > Logic::filter_targets_bf(const Filter& filter, std::optional< Team >
// opt_team)
//{
//   std::vector< Target > targets;
//
//   auto filter_lambda = [&](Team team) {
//      auto bf = m_state->board()->battlefield(team);
//      for(size_t i = 0; i < BATTLEFIELD_SIZE; ++i) {
//         if(const auto& opt_unit = bf.at(i); has_value(opt_unit) && filter(Target(opt_unit))) {
//            targets.emplace_back(Target(opt_unit));
//         }
//      }
//   };
//   if(opt_team.has_value()) {
//      filter_lambda(opt_team.value());
//   } else {
//      filter_lambda(BLUE);
//      filter_lambda(RED);
//   }
//   return targets;
//}
//
// std::vector< Target > Logic::filter_targets_camp(
//   const Filter& filter,
//   std::optional< Team > opt_team)
//{
//   std::vector< Target > targets;
//
//   auto filter_lambda = [&](Team team) {
//      auto camp = m_state->board()->camp(team);
//      for(const auto& unit : camp) {
//         if(filter(Target(unit))) {
//            targets.emplace_back(Target(unit));
//         }
//      }
//   };
//   if(opt_team.has_value()) {
//      filter_lambda(opt_team.value());
//   } else {
//      filter_lambda(BLUE);
//      filter_lambda(RED);
//   }
//   return targets;
//}
// std::vector< Target > Logic::filter_targets_hand(
//   const Filter& filter,
//   std::optional< Team > opt_team)
//{
//   std::vector< Target > targets;
//
//   auto filter_lambda = [&](Team team) {
//      auto hand = m_state->hand(team);
//      for(const auto& spell : hand) {
//         if(filter(Target(spell))) {
//            targets.emplace_back(Target(spell));
//         }
//      }
//   };
//   if(opt_team.has_value()) {
//      filter_lambda(opt_team.value());
//   } else {
//      filter_lambda(BLUE);
//      filter_lambda(RED);
//   }
//   return targets;
//}
//
// std::vector< Target > Logic::filter_targets_deck(
//   const Filter& filter,
//   std::optional< Team > opt_team)
//{
//   std::vector< Target > targets;
//
//   auto filter_lambda = [&](Team team) {
//      auto deck = m_state->deck(team);
//      for(const auto& spell : deck) {
//         if(filter(Target(spell))) {
//            targets.emplace_back(Target(spell));
//         }
//      }
//   };
//   if(opt_team.has_value()) {
//      filter_lambda(opt_team.value());
//   } else {
//      filter_lambda(BLUE);
//      filter_lambda(RED);
//   }
//   return targets;
//}
//
// std::vector< Target > Logic::filter_targets_board(
//   const Filter& filter,
//   std::optional< Team > opt_team)
//{
//   std::vector< Target > targets;
//
//   if(opt_team.has_value()) {
//      Team& team = opt_team.value();
//      targets = targets + filter_targets_bf(filter, team) + filter_targets_camp(filter, team);
//   } else {
//      targets = targets + filter_targets_bf(filter, BLUE) + filter_targets_camp(filter, BLUE)
//                + filter_targets_bf(filter, RED) + filter_targets_camp(filter, RED);
//   }
//   return targets;
//}
// std::vector< Target > Logic::filter_targets_everywhere(
//   const Filter& filter,
//   std::optional< Team > opt_team)
//{
//   std::vector< Target > targets;
//
//   if(opt_team.has_value()) {
//      Team& team = opt_team.value();
//
//      targets = targets + filter_targets_bf(filter, team) + filter_targets_camp(filter, team)
//                + filter_targets_hand(filter, team);
//   } else {
//      targets = targets + filter_targets_bf(filter, BLUE) + filter_targets_camp(filter, BLUE)
//                + filter_targets_hand(filter, BLUE) + filter_targets_bf(filter, RED)
//                + filter_targets_camp(filter, RED) + filter_targets_hand(filter, RED);
//   }
//   return targets;
//}
// void Logic::heal(Team team, const sptr< Unit >& unit, long amount)
//{
//   if(amount > 0) {
//      unit->heal(amount);
//      _trigger_event(m_subscribed_events::HealUnitEvent(team, unit, std::make_shared< long
//      >(amount)));
//   }
//}

//
// void Logic::_copy_grants(
//   const std::vector< sptr< Grant > >& grants,
//   const std::shared_ptr< Unit >& unit)
//{
//}
