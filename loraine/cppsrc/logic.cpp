#include "loraine/core/logic.h"

#include "loraine/core/action.h"
#include "loraine/core/gamestate.h"

Logic::Logic(const Logic& other)
    : m_state(other.m_handler),
      m_action_invoker(other.m_action_invoker->clone()),
      m_prev_action_invoker(other.m_prev_action_invoker->clone())
{
}

void Logic::request_action() const
{
   m_state->buffer().action.emplace_back(
      std::make_shared< actions::Action >(std::move(m_action_invoker->request_action(*state()))));
}

void Logic::cast(bool burst)
{
   auto& spell_stack = m_state->spell_stack();
   while(not spell_stack.empty()) {
      auto spell = spell_stack.back();
      spell_stack.pop_back();
      trigger_event< events::EventLabel::CAST >(spell->mutables().team, spell);
      if(burst) {
         break;
      }
   }
}
Status Logic::step()
{
   _start_round();
   bool flip_initiative = false;
   m_state->player(m_state->active_team()).flags().has_played = false;
   while(not flip_initiative) {
      request_action();
      flip_initiative = invoke_actions();
   }
   if(m_state->player(Team::BLUE).flags().pass && m_state->player(Team::RED).flags().pass) {
      _end_round();
   }
   m_state->turn() += 1;
   return check_status();
}
void Logic::play_event_triggers(const sptr< Card >& card)
{
   trigger_event< events::EventLabel::PLAY >(card->mutables().team, card);
   _trigger_daybreak_if(card);
   _trigger_nightfall_if(card);
}
void Logic::place_in_camp(const sptr< FieldCard >& card, const std::optional< size_t >& replaces)
{
   auto& camp = m_state->board().camp(card->mutables().team);
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
   Team team = card->mutables().team;
   auto daybreak = m_state->player(team).flags().is_daybreak;  // copy before overwriting
   m_state->player(team).flags().is_daybreak = false;  // if there was daybreak, there is none now
   if(daybreak && card->has_effect(events::EventLabel::DAYBREAK)) {
      trigger_event< events::EventLabel::DAYBREAK >(team, card);
   }
}
void Logic::_trigger_nightfall_if(const sptr< Card >& card)
{
   Team team = card->mutables().team;
   auto nightfall = m_state->player(team).flags().is_nightfall;  // copy before overwriting
   m_state->player(team).flags().is_nightfall = true;  // from now on there is certainly nightfall
   if(nightfall && card->has_effect(events::EventLabel::NIGHTFALL)) {
      trigger_event< events::EventLabel::NIGHTFALL >(team, card);
   }
}

void Logic::_start_round()
{
   auto& round = m_state->round();
   round += 1;
   for(Team team : {Team::RED, Team::BLUE}) {
      auto& flags = m_state->player(team).flags();
      flags.plunder_token = false;
      flags.is_daybreak = true;
      flags.is_nightfall = false;
      flags.attack_token = false;
      flags.scout_token = false;
      reset_pass(team);
      auto& mana = m_state->player(team).mana();
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
   auto& mana = m_state->player(team).mana();
   if(normal_mana) {
      mana.common = mana.gems;
   } else {
      mana.common = m_state->config().MAX_FLOATING_MANA;
   }
}

void Logic::draw_card(Team team)
{
   auto& deck = m_state->player(team).deck();
   if(deck.empty()) {
      _set_status(Status::win(opponent(team), false));
      return;
   }
   auto card_drawn = deck.pop();

   trigger_event< events::EventLabel::DRAW_CARD >(team, card_drawn);
   if(auto& hand = m_state->player(team).hand();
      hand.size() < m_state->config().HAND_CARDS_LIMIT) {
      hand.emplace_back(card_drawn);
   } else {
      obliterate(card_drawn);
   }
}

void Logic::give_managems(Team team, long amount)
{
   m_state->player(team).mana().gems += amount;
   trigger_event< events::EventLabel::GAIN_MANAGEM >(team, amount);
   _check_enlightenment(team);
}
void Logic::_check_enlightenment(Team team)
{
   auto& player = m_state->player(team);
   if(not player.flags().enlightened
      && player.mana().gems >= m_state->config().ENLIGHTENMENT_THRESHOLD) {
      player.flags().enlightened = true;
      trigger_event< events::EventLabel::ENLIGHTENMENT >(team);
   }
}

void Logic::spend_mana(const sptr< Card >& card)
{
   long common_mana_cost = card->mana_cost();
   auto& mana_resource = m_state->player(card->mutables().team).mana();

   if(card->is_spell()) {
      common_mana_cost = static_cast< size_t >(card->mana_cost() - mana_resource.floating);
      // we have either spent all spell mana and thus reach 0, or there will be leftovers, which
      // will see `-common_mana_cost` > 0
      mana_resource.floating = std::max(0L, -common_mana_cost);
   }
   if(common_mana_cost > 0) {
      // there is some mana left to pay
      if(common_mana_cost > mana_resource.common) {
         throw std::logic_error(
            "The played spell " + card->immutables().name + "costs more than the total mana of the player. An invalid action "
            "wasn't recognized.");
      }
      mana_resource.common -= common_mana_cost;
   }
}

void Logic::process_camp_queue(Team team)
{
   // the logic of LOR for deciding how many units in the m_queue fit into the
   // camp goes by 2 different modes:
   // 1) default play mode, and
   // 2) battle resolution mode

   // In 1) the card will be obliterated, if there is no space currently on the board left,
   // after having considered all previous units in the m_queue. Otherwise, it is placed
   // in the camp. When a unit is summoned when attack is declared (e.g. Kalista summoning
   // Rekindler, who in turn summons another Kalista again) and the EXPECTED number of
   // surviving units is less than MAX_NR_CAMP_SLOTS (e.g. because some of the attacking
   // units are ephemeral and expected to die on strike), then that unit would be placed
   // in camp immediately. If contrary to expectation, some units manage to survive (e.g.
   // through "Death's Hand" floating removing ephemeral or the ephemeral was shadow-blocked),
   // then, starting from the end, any unit of the battlefield, which no longer finds space
   // in the camp, is obliterated (this is handled in the method 'retreat_to_camp').
   auto& board = m_state->board();
   auto& queue = board.camp_queue(team);
   if(queue.empty()) {
      return;
   }

   size_t camp_units_count = board.count_occupied_spots(team, true);
   size_t bf_units_count = board.count_units(team, false, [](const sptr< Card >& unit) {
      // ephemerals are expected to be gone after striking, which is why they are excluded from
      // counting as 'camp returning' units (at least this has been observed in the actual gameplay)
      return not unit->has(Keyword::EPHEMERAL);
   });
   size_t units_in_queue = queue.size();
   auto& camp = board.camp(team);
   if(auto rem_slots = board.max_size_camp() - camp_units_count - bf_units_count; rem_slots > 0) {
      size_t curr_idx = camp_units_count - 1;
      for(size_t counter = 0; counter < std::min(units_in_queue, rem_slots); ++counter) {
         camp.at(curr_idx) = queue.front();
         queue.pop();
         curr_idx += 1;
      }
   }
}

Status Logic::check_status()
{
   const auto& cfg = m_state->config();
   if(m_state->round() > cfg.MAX_ROUNDS) {
      m_state->m_status = Status::TIE;
   }
   auto& player_blue = m_state->player(Team::BLUE);
   auto& player_red = m_state->player(Team::RED);
   if(player_blue.nexus().health() < 1) {
      if(player_red.nexus().health() < 1) {
         _set_status(Status::TIE);
      }
      _set_status(Status::RED_WINS_NEXUS);
   } else if(player_red.nexus().health() < 1) {
      _set_status(Status::BLUE_WINS_NEXUS);
   }
   _set_status(Status::ONGOING);
   return m_state->m_status;
}

void Logic::pass()
{
   m_state->player(m_state->active_team()).flags().pass = true;
}
void Logic::reset_pass(Team team)
{
   m_state->player(team).flags().pass = false;
}

void Logic::reset_pass()
{
   m_state->player(m_state->active_team()).flags().pass = false;
}
void Logic::_set_status(Status status)
{
   if(m_state->m_status == Status::ONGOING) {
      // once the status is set to anything but ongoing, it is frozen
      m_state->m_status = status;
   }
   m_state->status().mark_checked();
}
bool Logic::invoke_actions()
{
   auto& action_buffer = m_state->buffer().action;
   bool flip_initiative = true;
   while(not action_buffer.empty()) {
      auto& action = action_buffer.back();
      m_state->commit_to_history(std::make_unique< ActionRecord >(action));
      flip_initiative = m_action_invoker->handle(*action);
      action_buffer.pop_back();
   }
   return flip_initiative;
}

void Logic::resolve()
{
   // cast all the spells on the spell stack first
   cast(false);
   // then process the combat if necessary
   auto overwhelm_if = [&](sptr< Unit > unit_att, long dmg) {
      if(dmg > 0 && unit_att->has(Keyword::OVERWHELM)
         && m_state->attacker() == unit_att->mutables().team) {
         strike_nexus(unit_att, dmg);
      }
   };

   if(in_combat()) {
      auto attacker = m_state->attacker().value();
      auto defender = Team(1 - attacker);
      auto& bf_att = m_state->board().battlefield(attacker);
      auto& bf_def = m_state->board().battlefield(defender);
      for(auto pos = 0U; pos < m_state->board().max_size_bf(); ++pos) {
         if(pos >= bf_att.size()) {
            // we do this dynamic check incase a strike or another effect might have summoned
            // another attacker
            break;
         }
         auto unit_att = bf_att.at(pos);
         auto unit_def = bf_def.at(pos);

         if(utils::has_value(unit_def)) {
            if(unit_def->unit_mutables().alive) {
               bool quick_attacks = unit_att->has(Keyword::QUICK_ATTACK);
               bool double_attacks = unit_att->has(Keyword::DOUBLE_ATTACK);

               if(quick_attacks || double_attacks) {
                  // first the attacker hits the defender, any surplus is potential overwhelm_if dmg
                  overwhelm_if(unit_att, strike(unit_att, unit_def));
                  // check whether to kill defending unit
                  if(unit_def->health() == 0) {
                     kill_unit(unit_def, unit_att);
                  }
                  if(unit_def->unit_mutables().alive) {
                     if(double_attacks) {
                        // a double attacking unit attacks again after a quick attack
                        overwhelm_if(unit_att, strike_mutually(unit_att, unit_def)[0]);
                     }
                     // now the defender strikes back
                     strike(unit_def, unit_att);
                  }

               } else {
                  overwhelm_if(unit_att, strike_mutually(unit_att, unit_def)[0]);
               }

               // check whether to kill the attacking unit
               if(unit_att->health() == 0) {
                  kill_unit(unit_att, unit_att);
               }
               // check whether to kill defending unit
               if(unit_def->health() == 0) {
                  kill_unit(unit_def, unit_att);
               }
            } else {
               // if the blocking unit is already dead, the attacker could still overwhelm
               overwhelm_if(unit_att, unit_att->power());
            }
         } else {
            // if there is no defender to block the attack, the attacker
            // strikes the nexus
            strike_nexus(unit_att, unit_att->power());
         }

         retreat_to_camp(attacker);
         retreat_to_camp(defender);
      }
   }
   transition< IdleActionHandler >();
}
long Logic::strike(const sptr< Unit >& unit_att, sptr< Unit >& unit_def)
{
   auto dmg = unit_att->power();
   long surplus_dmg = 0;
   if(dmg > 0) {
      trigger_event< events::EventLabel::STRIKE >(unit_att->mutables().team, unit_att, unit_def);
      surplus_dmg = dmg - damage_unit(unit_def, unit_att, dmg);
   }
   return surplus_dmg;
}
SymArr< long > Logic::strike_mutually(const sptr< Unit >& unit1, sptr< Unit >& unit2)
{
   SymArr< long > dmg_taken{0, 0};
   SymArr< long > surplus_dmg{0, 0};
   std::function< void() > trigger_strike_1 = [&]() {
      trigger_event< events::EventLabel::STRIKE >(unit1->mutables().team, unit1, unit2);
   };
   std::function< void() > trigger_strike_2 = [&]() {
      trigger_event< events::EventLabel::STRIKE >(unit2->mutables().team, unit2, unit1);
   };
   if(auto dmg = unit1->power(); dmg > 0) {
      dmg_taken[0] = unit2->take_damage(unit1, dmg);
      surplus_dmg[0] = dmg - dmg_taken[0];
   } else {
      trigger_strike_1 = []() {};
   }
   if(auto dmg = unit2->power(); dmg > 0) {
      dmg_taken[1] = unit1->take_damage(unit2, dmg);
      surplus_dmg[1] = dmg - dmg_taken[1];
   } else {
      trigger_strike_2 = []() {};
   }
   trigger_strike_1();
   trigger_strike_2();

   if(auto dmg = dmg_taken[0]; dmg > 0) {
      trigger_event< events::EventLabel::UNIT_DAMAGE >(unit1->mutables().team, unit1, unit2, dmg);
   }
   if(auto dmg = dmg_taken[1]; dmg > 0) {
      trigger_event< events::EventLabel::UNIT_DAMAGE >(unit2->mutables().team, unit2, unit1, dmg);
   }
   return surplus_dmg;
}
void Logic::init_attack(Team team)
{
   m_state->player(team).flags().attack_token = false;
   trigger_event< events::EventLabel::ATTACK >(team);

   auto& attacker_bf = m_state->board().battlefield(team);
   // Check for support get taking place:
   // we need to loop over all potential indices, since attack and support get might
   // summon further attackers onto the battlefield, which changes the battlefield container
   // sizes (and invalidates existing iterators)
   for(int i = 0; i < m_state->board().max_size_bf(); ++i) {
      if(not (i < attacker_bf.size() - 1)) {
         // once we reach the last unit on board, we dont check for support get anymore
         break;
      }
      if(attacker_bf[i]->has(Keyword::SUPPORT)) {
         trigger_event< events::EventLabel::SUPPORT >(team, attacker_bf[i], attacker_bf[i + 1]);
      }
   }
   transition< CombatActionPhase >();
}
void Logic::init_block(Team team)
{
   trigger_event< events::EventLabel::BLOCK >(team);
}
long Logic::damage_unit(const sptr< Unit >& unit, const sptr< Card >& cause, long dmg)
{
   long dmg_taken = unit->take_damage(cause, dmg);
   trigger_event< events::EventLabel::UNIT_DAMAGE >(
      cause->mutables().team, cause, unit, dmg_taken);
   return dmg_taken;
}
void Logic::kill_unit(const sptr< Unit >& killed_unit, const sptr< Card >& cause)
{
   killed_unit->kill(cause);
   if(not killed_unit->unit_mutables().alive) {
      // we need to check for the unit being truly dead, in case it had an
      // e.g. last breath effect (The Immortal Fire), which kept it alive, or a level up effect
      // (Tryndamere) etc.
      trigger_event< events::EventLabel::SLAY >(cause->mutables().team, cause, killed_unit);
      m_state->send_to_graveyard(killed_unit);
      _remove(killed_unit);
   }
}
void Logic::_remove(const sptr< Card >& card)
{
   Team team = card->mutables().team;
   unsubscribe_effects(card);
   // remove the unit from camp
   if(card->is_fieldcard()) {
      auto loc = card->mutables().location;
      if(loc == Zone::CAMP) {
         auto camp = m_state->board().camp(team);
         camp.erase(std::next(camp.begin(), card->mutables().index));
      }
      // if it is on the battlefield, then we let the retreat to camp method clean up dead units
   }
}
void Logic::unsubscribe_effects(const sptr< Card >& card)
{
   //   unsubscribe_effects_impl< events::n_events - 1 >(card);

   for(auto& [label, effect_vec] : card->effects()) {
      for(auto& effect : effect_vec) {
         auto& event = m_state->event(label);
         effect->disconnect();
      }
   }
}
void Logic::subscribe_effects(
   const sptr< Card >& card, IEffect::RegistrationTime registration_time)
{
   //   subscribe_effects_impl< events::n_events - 1 >(card, registration_time);
   for(auto& [label, effect_vec] : card->effects()) {
      auto& event = m_state->event(label);
      for(auto& effect : effect_vec) {
         if(effect->registration_time() == registration_time) {
            effect->connect(event);
         }
      }
   }
}
void Logic::strike_nexus(const sptr< Unit >& striking_unit, long dmg)
{
   if(dmg > 0) {
      m_state->player(striking_unit->mutables().team).nexus().add_health(striking_unit, dmg);
   }
}

// std::vector< sptr< Card > > Logic::_draw_n_cards(Team team, int n)
//{
//   std::vector< sptr< Card > > hand(n);
//   auto& deck = m_handler->deck(team);
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
//   auto mull_act_blue = m_agents[BLUE]->choose_mulligan(*m_handler, hand_blue);
//   auto mull_act_red = m_agents[RED]->choose_mulligan(*m_handler, hand_red);
//
//   std::array< std::array< bool, INITIAL_HAND_SIZE >, 2 > replace = {
//      mull_act_blue->replace_decisions(), mull_act_red->replace_decisions()};
//
//   for(int p = BLUE; p != RED; ++p) {
//      auto& curr_hand = hands.at(p);
//      GameState::HandType new_hand;
//      auto nr_cards_to_replace = 0;
//      auto replace_for_p = replace.at(p);
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
//      m_handler->hand(new_hand, Team(p));
//   }
//}
void Logic::_end_round()
{
   // first let all m_effects fire that state an effect with the "Round End" keyword
   auto active_team = m_state->active_team();
   auto passive_team = opponent(active_team);
   trigger_event< events::EventLabel::ROUND_END >(active_team, m_state->round());

   SymArr< std::vector< sptr< Unit > > > regenerating_units;
   auto end_round_proc = [&](Team team) {
      // kill ephemeral units
      for(auto& unit : m_state->board().camp(team)) {
         if(unit->has(Keyword::EPHEMERAL)) {
            kill_unit(to_unit(unit), unit);
            continue;
         }
         // remove temporary grants
         // undo temporary loraine/buffs/nerfs and possibly heal the units if applicable
         auto temp_grants = unit->mutables().grants_temp;
         for(auto&& grant : temp_grants) {
            grant->undo();
         }
         temp_grants.clear();

         // REGENERATION units will regenerate after removing grants etc.
         if(unit->has(Keyword::REGENERATION)) {
            regenerating_units[team].emplace_back(to_unit(unit));
         }
      }
      // store floating mana if available
      auto& mana = m_state->player(team).mana();
      mana.floating += mana.common;
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
      cause->mutables().team, unit, cause, true_amount);
}
void Logic::retreat_to_camp(Team team)
{
   auto& bf = m_state->board().battlefield(team);
   auto& camp = m_state->board().camp(team);
   std::decay_t< decltype(bf) > bf_stack_buffer;
   bf_stack_buffer.reserve(bf.size());
   // we reverse the bf container, because popping from the end of a vector is performant, while
   // popping from the beginning is not, and we need to loop over the bf from the beginning, since
   // the units return to camp from left to right
   for(const auto& unit : utils::reverse(bf)) {
      if(unit->unit_mutables().alive) {
         bf.emplace_back(unit);
      }
      // if it isn't alive, then the kill_unit method should have already sent it to the graveyard
      // and unsubscribed its get. The pointer on the battlefield was only kept for
      // deciding striking mechanisms during combat resolution
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
void Logic::restore_previous_invoker()
{
   utils::throw_if_no_value(
      m_prev_action_invoker, "Previous action invoker pointer holds no value.");
   m_action_invoker = std::move(m_prev_action_invoker);
   m_prev_action_invoker = nullptr;
}

void Logic::summon(const sptr< FieldCard >& card, std::optional< size_t > replaces, bool to_bf, bool played)
{
   auto& camp = m_state->board().camp(card->mutables().team);
   if(utils::has_value(replaces)) {
      size_t replace_idx = replaces.value();
      obliterate(camp[replace_idx]);
      camp[replace_idx] = card;
   } else {
      camp.emplace_back(card);
   }
   if(played) {
      play_event_triggers(card);
   } else {
      trigger_event<events::EventLabel::SUMMON>(card->mutables().team, card);
   }
}
// void Logic::summon(const sptr< Unit >& unit, bool to_bf)
//{
//   m_handler->board()->add_to_camp_queue(unit);
//   Team team = unit->mutables().team;
//   process_camp_queue(team);
//   m_event_listener.register_card(unit);
//   _trigger_event(m_subscribed_events::SummonEvent(team, unit));
//}
// void Logic::summon_to_battlefield(const sptr< Unit >& unit)
//{
//   Team team = unit->mutables().team;
//   auto bf = m_handler->board()->battlefield(team);
//   auto curr_unit_count = m_handler->board()->count_occupied_spots(team, false);
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
//   Team team = unit->mutables().team;
//   auto copied_unit = std::make_shared< Unit >(*unit);
//   m_handler->board()->add_to_camp_queue(unit);
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
//      auto bf = m_handler->board()->battlefield(team);
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
//      auto camp = m_handler->board()->camp(team);
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
//      auto hand = m_handler->hand(team);
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
//      auto deck = m_handler->deck(team);
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
