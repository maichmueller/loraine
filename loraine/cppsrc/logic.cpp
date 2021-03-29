#include "core/logic.h"

#include "core/action.h"
#include "core/state.h"

Logic::Logic(const Logic& other)
    : m_state(other.m_state),
      m_action_handler(other.m_action_handler->clone()),
      m_prev_action_handler(other.m_prev_action_handler->clone())
{
}

actions::Action Logic::request_action() const
{
   return m_action_handler->request_action(*state());
}

void Logic::cast(const sptr< Spell >& spell)
{
   return cast(std::vector< sptr< Spell > >{spell});
}

void Logic::play_spell(const sptr< Spell >& spell)
{
   spell->uncover();

   // pay the mana cost

   size_t cost = spell->mana_cost();
   auto* mana_resource = m_state->player(spell->mutables().owner).mana();
   // the cost that will have to be paid with normal mana
   long leftover_cost = cost - mana_resource->floating;
   mana_resource->floating = std::max(0L, -leftover_cost);
   if(leftover_cost > mana_resource->common) {
      throw std::logic_error(
         "The moved spell costs more than the total mana of the player. An invalid action "
         "wasn't recognized.");
   }
   mana_resource->common -= leftover_cost;

   trigger_event< events::EventLabel::PLAY >(spell->mutables().owner, spell);
   _trigger_daybreak_if(spell);
   _trigger_nightfall_if(spell);
}
void Logic::_resolve_spell_stack(bool burst)
{
   auto* spell_stack = m_state->spell_stack();
   if(burst) {
      m_state->logic()->cast(spell_stack->back());
      spell_stack->pop_back();
   } else {
      m_state->logic()->cast(*spell_stack);
   }
}
Status Logic::step()
{
   _start_round();
   bool flip_initiative = false;
   while(not flip_initiative) {
      if(auto* targ_buffer = m_state->targeting_buffer(); targ_buffer->empty()) {
         // if no targets are currently required, then simply ask the current player for action
         auto action = request_action();
         flip_initiative = action.execute(*m_state);
      } else {
         // the targeting buffer is currently not empty so we demand targets
         transition_action_handler< TargetModeHandler >();
         while(not targ_buffer->empty()) {
            auto action = request_action();
            flip_initiative = action.execute(*m_state);
         }
         restore_previous_handler();
      }
   }
   if(m_state->requires_resolution()) {
      _resolve_spell_stack(false);
      _resolve_battle();
      m_state->requires_resolution(false);
   } else if(
      m_state->player(Team::BLUE).flags()->pass && m_state->player(Team::RED).flags()->pass) {
      _end_round();
   }
   m_state->turn() += 1;
   return check_status();
}
void Logic::play(const sptr< FieldCard >& card, std::optional< size_t > replaces)
{
   card->uncover();

   // pay the mana cost

   size_t cost = card->mana_cost();
   auto team = card->mutables().owner;
   auto* mana_resource = m_state->player(team).mana();
   // the cost that will have to be paid with normal mana
   if(cost > mana_resource->common) {
      throw std::logic_error(
         "The played card costs more than the normal mana of the player. An invalid action "
         "wasn't recognized.");
   }
   mana_resource->common -= cost;

   auto& camp = *m_state->board()->camp(team);
   if(utils::has_value(replaces)) {
      size_t replace_idx = replaces.value();
      obliterate(camp[replace_idx]);
      camp[replace_idx] = card;
   } else {
      camp.emplace_back(card);
   }
   // trigger play events
   trigger_event< events::EventLabel::PLAY >(card->mutables().owner, card);
   _trigger_daybreak_if(card);
   _trigger_nightfall_if(card);
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
            "The played card " + card->immutables().name + "costs more than the total mana of the player. An invalid action "
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
//   // 1) default play mode, and
//   // 2) battle resolution mode
//
//   // In 1) the card will be obliterated, if there is no space currently on the board left,
//   // after having considered all previous units in the m_queue. Otherwise, it is placed
//   // in the camp. When a common is summoned when attack is declared (e.g. Kalista summoning
//   // Rekindler, who in turn summons another Kalista again) and the EXPECTED number of
//   // surviving units is less than MAX_NR_CAMP_SLOTS (e.g. because some of the attacking
//   // units are ephemeral and expected to die on strike), then that common would be placed
//   // in camp immediately. If contrary to expectation, some units manage to survive (e.g.
//   // through "Death's Hand" floating removing ephemeral or the ephemeral was shadow-blocked),
//   // then, starting from the end, any common of the battlefield, which no longer finds space
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
//
// void Logic::damage_nexus(size_t amount, Team team)
//{
//   m_state->nexus_health(team) -= amount;
//}
// void Logic::heal_nexus(size_t amount, Team team)
//{
//   m_state->nexus_health(team) = std::min(
//      m_state->nexus_health(team) + static_cast< long >(amount),
//      static_cast< long >(m_state->config().START_NEXUS_HEALTH));
//}
// bool Logic::round_ended() const
//{
//   return m_state->pass_flag(Team::BLUE) && m_state->pass_flag(Team::RED);
//}
//
bool Logic::pass()
{
   m_state->player(m_state->active_team()).flags()->pass = true;
   return round_ended();
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


//
// bool Logic::_do_action(const sptr< Action >& action)
//{
//   auto action_type = action->action_label();
//   bool flip_initiative = true;  // whether the other team gets to act next.
//   if(action_type == ActionLabel::PASS) {
//      // this case has to be handled separately to combine the pass counter
//      // reset for all other cases
//      bool round_ends = m_state->pass();
//      if(round_ends) {
//         end_round();
//         start_round();
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
//               auto spell = to_spell(card);
//               play_spell();
//               if(spell->has_keyword(Keyword::BURST)) {
//                  _resolve_spell_stack(true);
//                  flip_initiative = false;
//               }
//            } else {
//               play(card, cast_action->replacement());
//            }
//         }
//
//         case PLACE_UNIT: {
//            auto cast_action = std::dynamic_pointer_cast< PlaceUnitAction >(action);
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
//         case PLACE_SPELL: {
//            auto cast_action = std::dynamic_pointer_cast< PlaceSpellAction >(action);
//            auto spell = cast_action->spell();
//            if(not _move_spell(spell, cast_action->to_stack())) {
//               // the floating was supposed to be moved to the stack, but the team cancelled
//               _move_spell(spell, false);
//               flip_initiative = false;
//            } else {
//               flip_initiative = _do_action(
//                  std::make_shared< PlayAction >(m_state->round(), spell->mutables().owner,
//                  spell));
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
//   m_state->commit_to_history(action, BLUE);
//   return flip_initiative;
//}
// void Logic::_activate_battlemode(Team attack_team)
//{
//   m_state->set_battle_mode(true);
//   m_state->attacker(attack_team);
//   auto& bf = m_state->board()->battlefield(attack_team);
//   auto check_att_effects = [&](const sptr< Card >& unit) {
//      if(unit->has_keyword(Keyword::ATTACK)) {
//         _trigger_event(events::AttackEvent(attack_team, unit));
//      }
//   };
//   auto check_supp_effects = [&](const sptr< Card >& unit, const sptr< Card >& next_unit) {
//      if(unit->has_keyword(Keyword::SUPPORT)) {
//         _trigger_event(events::SupportEvent(attack_team, unit, next_unit));
//      }
//   };
//
//   for(int i = 0; i < BATTLEFIELD_SIZE - 1; ++i) {
//      if(has_value(bf[i])) {
//         const auto& attack_unit = bf[i];
//         check_att_effects(attack_unit);
//         if(auto next_unit = bf[i + 1]; has_value(next_unit)) {
//            check_supp_effects(attack_unit, next_unit);
//         }
//      }
//   }
//   if(auto last_att = bf[BATTLEFIELD_SIZE]; has_value(last_att)) {
//      check_att_effects(last_att);
//   }
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
//      _trigger_event(events::ScoutEvent(attack_team));
//   } else {
//      m_state->token_attack(false, attack_team);
//   }
//}
// void Logic::_deactivate_battlemode()
//{
//   m_state->set_battle_mode(false);
//   m_state->reset_attacker();
//}
// void Logic::_resolve_spell_stack(bool burst)
//{
//   auto& spell_stack = m_state->spell_stack();
//   if(burst) {
//      auto last_spell = spell_stack.back();
//      spell_stack.pop_back();
//      cast(last_spell);
//   } else {
//      _cast_spellstack();
//      if(m_state->in_battle_mode()) {
//         _resolve_battle();
//      }
//   }
//}
// void Logic::_resolve_battle()
//{
//   m_state->set_battle_resolution_mode(true);
//   auto attacker = m_state->attacker().value();
//   auto defender = Team(1 - attacker);
//   auto& battlefield_att = m_state->board()->battlefield(attacker);
//   auto& battlefield_def = m_state->board()->battlefield(defender);
//   for(auto pos = 0U; pos < m_state->board()->count_units(attacker, false); ++pos) {
//      auto unit_att_opt = battlefield_att.at(pos);
//
//      if(has_value(unit_att_opt)) {
//         // if this spot on the battlefield sees an actual common posted for
//         // attack we need to make it strike
//         auto unit_att = to_unit(unit_att_opt);
//
//         auto unit_def_opt = battlefield_def.at(pos);
//         if(has_value(unit_def_opt)) {
//            // if there is a defender posted on the same spot of the
//            // attacker, it needs to battle the attacker
//            auto unit_def = to_unit(unit_def_opt);
//
//            if(unit_def->unit_mutables().alive) {
//               bool quick_attacks = unit_att->has_keyword(Keyword::QUICK_ATTACK);
//               bool double_attacks = unit_att->has_keyword(Keyword::DOUBLE_ATTACK);
//
//               long health_def_after = 0;
//               long attack_power = unit_att->power();
//
//               if(quick_attacks || double_attacks) {
//                  strike(unit_att, unit_def);
//                  if(unit_def->health() > 0) {
//                     // we cant check for the common being alive, but have to
//                     // check via the health instead, because the common is
//                     // only killed later on. The health is an intermediary.
//                     if(double_attacks) {
//                        // a double attacking common attacks again after a quick
//                        // attack
//                        strike(unit_att, unit_def);
//                     }
//                     strike(unit_def, unit_att);
//                  }
//
//               } else {
//                  strike(unit_att, unit_def);
//                  strike(unit_def, unit_att);
//               }
//
//               // check whether to kill the attacking common
//               if(unit_att->health() == 0) {
//                  kill_unit(defender, unit_att, unit_att);
//               }
//               // check whether to kill defending common
//               if(unit_def->health() == 0) {
//                  kill_unit(attacker, unit_def, unit_att);
//               }
//            }
//         } else {
//            // if there is no defender to block the attack, the attacker
//            // strikes the nexus
//            nexus_strike(defender, unit_att);
//         }
//      }
//      retreat_to_camp(attacker);
//      retreat_to_camp(defender);
//      m_state->set_battle_mode(false);
//   }
//   m_state->set_battle_resolution_mode(false);
//}
// void Logic::strike(const sptr< Unit >& unit_att, sptr< Unit >& unit_def)
//{
//   sptr< long > damage = std::make_shared< long >(unit_att->power());
//   if(*damage > 0) {
//      _trigger_event(events::StrikeEvent(unit_att->mutables().owner, unit_att, unit_def));
//      damage_unit(unit_att, unit_def, damage);
//   }
//   if(m_state->in_battle_mode() && m_state->attacker() == unit_att->mutables().owner
//      && unit_att->has_keyword(Keyword::OVERWHELM)) {
//      nexus_strike(unit_def->mutables().owner, damage, unit_att);
//   }
//}
// void Logic::damage_unit(
//   const sptr< Card >& cause,
//   const sptr< Unit >& unit,
//   const sptr< long >& damage)
//{
//   long health_def = unit->health();
//   trigger_event(events::UnitDamageEvent(cause->mutables().owner, cause, Target(unit), damage));
//   unit->take_damage(*damage);
//   // store any surplus damage in the damage ptr (e.g. for overwhelm dmg calc)
//   *damage += -health_def;
//}
// void Logic::kill_unit(Team killer, const sptr< Unit >& killed_unit, const sptr< Card >& cause)
//{
//   killed_unit->kill();
//   _trigger_event(events::DieEvent(killer, Target(killed_unit), cause));
//   if(not killed_unit->unit_mutables().alive) {
//      // we need to check for the card being truly dead, in case it had an
//      // e.g. last breath effect, which kept it alive or level up effect (Tryndamere)
//      m_state->to_graveyard(killed_unit);
//   }
//   _remove(killed_unit);
//}
//
// void Logic::nexus_strike(const sptr< Unit >& striking_unit)
//{
//   Team attacked_nexus = opponent(striking_unit->mutables().owner);
//   sptr< long > att_power = std::make_shared< long >(striking_unit->power());
//   if(*att_power > 0) {
//      _trigger_event(events::NexusStrikeEvent(
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
//      State::HandType new_hand;
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
// void Logic::_end_round()
//{
//   // first let all m_effects trigger that state an effect with the "Round End" keyword
//   auto active_team = m_state->active_team();
//   auto passive_team = Team(1 - active_team);
//   _trigger_event(events::RoundEndEvent(active_team, std::make_shared< long >(m_state->round())));
//
//   SymArr< std::vector< sptr< Unit > > > regenerating_units;
//   auto end_round_procedure = [&](Team team) {
//      // kill ephemeral units
//      for(auto& unit : m_state->board()->camp(team)) {
//         if(unit->has_keyword(Keyword::EPHEMERAL)) {
//            kill_unit(team, to_unit(unit));
//         }
//         // remove temporary grants
//         // undo temporary buffs/nerfs and possibly heal the units if applicable
//         auto temp_grants = unit->mutables().grants_temp;
//         for(auto&& grant : temp_grants) {
//            grant->undo();
//         }
//         temp_grants.clear();
//
//         // REGENERATION units will regenerate after removing grants etc.
//         if(unit->has_keyword(Keyword::REGENERATION)) {
//            regenerating_units[team].emplace_back(to_unit(unit));
//         }
//      }
//
//      // float floating mana if available
//      m_state->floating_mana(m_state->floating_mana(team) + m_state->mana(team), team);
//   };
//
//   auto regenerate_units = [&](Team team) {
//      // regenerate the units with regeneration
//      for(auto& unit : regenerating_units[team]) {
//         if(unit->unit_mutables().alive) {
//            heal(unit->mutables().owner, unit, unit->unit_mutables().damage);
//         }
//      }
//   };
//   end_round_procedure(active_team);
//   end_round_procedure(passive_team);
//
//   regenerate_units(active_team);
//   regenerate_units(passive_team);
//}

// void Logic::summon(const sptr< Unit >& unit, bool to_bf)
//{
//   m_state->board()->add_to_camp_queue(unit);
//   Team team = unit->mutables().owner;
//   process_camp_queue(team);
//   m_event_listener.register_card(unit);
//   _trigger_event(events::SummonEvent(team, unit));
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
//      _trigger_event(events::SummonEvent(team, unit));
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
//   _trigger_event(events::SummonEvent(team, unit));
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
//      for(const auto& card : hand) {
//         if(filter(Target(card))) {
//            targets.emplace_back(Target(card));
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
//      for(const auto& card : deck) {
//         if(filter(Target(card))) {
//            targets.emplace_back(Target(card));
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
//      _trigger_event(events::HealUnitEvent(team, unit, std::make_shared< long >(amount)));
//   }
//}
// bool Logic::check_daybreak(Team team) const
//{
//   const auto& history = m_state->history()->at(team).at(m_state->round());
//   // if no play action is found in the history (actions are committed to history only
//   // after having been processed), then daybreak is happening.
//   return std::find_if(
//             history.begin(),
//             history.end(),
//             [](const sptr< Action >& action) {
//                return action->action_label() == ActionLabel::PLAY;
//             })
//          == history.end();
//}
// bool Logic::check_nightfall(Team team) const
//{
//   const auto& history = m_state->history(team)->at(m_state->round());
//   // if any play action is found in the current history (actions are committed to history only
//   // after having been processed), then nightfall is happening.
//   return std::find_if(
//             history.begin(),
//             history.end(),
//             [](const sptr< Action >& action) {
//                return action->action_label() == ActionLabel::PLAY;
//             })
//          != history.end();
//}
// void Logic::_remove(const sptr< Card >& card)
//{
//   m_event_listener.unregister_card(card);
//   if(auto loc = card->mutables().location; loc == Location::CAMP) {
//      algo::remove_element(m_state->board()->camp(card->mutables().owner), card);
//   } else if(loc == Location::BATTLEFIELD) {
//      algo::remove_element(m_state->board()->battlefield(card->mutables().owner), card);
//   } else if(loc == Location::HAND) {
//      algo::remove_element(m_state->hand(card->mutables().owner), card);
//   }
//}
//
// void Logic::_copy_grants(
//   const std::vector< sptr< Grant > >& grants,
//   const std::shared_ptr< Unit >& unit)
//{
//}
// void Logic::_cast_spellstack()
//{
//   auto stack = m_state->spell_stack();
//   // iterate from the end, since the stack is LIFO
//   for(auto& spell : reverse(*stack)) {
//      if(spell->check_cast_condition(*this)) {
//         trigger_event< events::EventLabel::CAST >(spell->mutables().owner, spell);
//      }
//   }
//   stack.clear();
//}
