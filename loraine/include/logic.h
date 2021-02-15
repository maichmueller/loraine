

#ifndef LORAINE_LOGIC_H
#define LORAINE_LOGIC_H

#include <grants/grant.h>

#include <array>

#include "events/event.h"
#include "events/event_types.h"

// forward declare
class State;
class Action;

/*
 * Class handling the logic of the LoR State. It has different modes for handling
 * a given Action object using the 'State' pattern (unrelated to class 'State').
 * The context for the Logic is the State object on which to act.
 *
 * In general the base logic provides the game logic of LoR, with the subclasses
 * implementing different methods of dealing with actions.
 *
 * The specific subclasses are to implement 6 different game modes:
 *    1. Active-mode: Handle actions for
 *          i)    moving units onto the battlefield
 *          ii)   placing spells on the stack
 *          iii)  playing a field card or the placed spells or a burst spell
 *          iv)   passing
 *    2. Attack-mode: Handle actions for
 *          i)    declaring an attack with the moved units
 *          ii)   dragging opponent units onto the battlefield
 *          iii)  moving (fast, burst) spells onto the stack
 *          iv)   playing the placed spells or a burst spell
 *    3. Block-mode: Handle actions for
 *          i)    moving units onto the attacking positions on the battlefield
 *          ii)   moving (fast, burst) spells onto the stack
 *          iii)  playing a burst spell
 *    4. Battle-mode: Handle actions for
 *          i)    placing a (fast, burst) spell on the stack
 *          ii)   playing the placed spells or a burst spell
 *          ii)   accepting
 *    5. Passive-mode: Handle actions for
 *          i)    placing spells on the stack
 *          ii)   playing a field card or spell
 *          iii)  passing
 *    6. Targeting-mode: Handle actions for
 *          i)    targeting a specific amount of cards
 *    7. Mulligan-mode: Handle actions for
 *          i)    mulligan the starting hand cards
 *
 */
class LogicMode {
   LogicMode() : m_state(nullptr) {}
   explicit LogicMode(State& state) : m_state(&state) {}

   virtual ~LogicMode() = default;

   void context(State& state) { m_state = &state;}
   auto context() { return m_state;}

   // Virtual methods to distinguish in the sub-modes.

   virtual void handle(const sptr<Action>& action) = 0;
   virtual bool is_valid(const sptr<Action>& action) = 0;

   // Beginning of shared logic implementations

   void incr_managems(Player player, long amount = 1);

   // play a unit or landmark card onto the board
   void play(const sptr< Card >& card, std::optional< size_t > replaces);
   // play all the spells that have been added to the spellstack
   void play_spells();
   void cast(const sptr< Spell >& spell);
   void summon(const sptr< Unit >& unit);

   void summon_to_battlefield(const sptr< Unit >& unit);
   void summon_exact_copy(const sptr< Unit >& unit);
   void create_card(Player player, size_t card_id);

   void create_copy(Player player, const sptr< Card >& card, bool exact_copy = false);
   void recall(Player recaller, const sptr< Card >& recalled_card);

   void deal_damage_to_unit(
      const sptr< Card >& cause, const sptr< Unit >& unit, const sptr< long >& damage);

   void heal(Player player, const sptr< Unit >& unit, long amount);

   void nexus_strike(Player attacked_nexus, const sptr< Unit >& striking_unit);

   void strike(const sptr< Unit >& unit_att, sptr< Unit >& unit_def);

   void kill_unit(
      Player killer, const sptr< Unit >& killed_unit, const sptr< Card >& cause = {});

   /*
    * An api for triggering an events mainly externally. Which events is supposed to be triggered
    * needs to be known at compile time.
    */
   template < events::EventType event_type, typename... Params >
   constexpr inline void trigger_event(Params... params);

   template < Location range >
   std::vector< Target > filter_targets(
      State& state, const Filter& filter, std::optional< Player > opt_player);

   void retreat_to_camp(State& state, Player player);

   void process_camp_queue(State& state, Player player);
   void draw_card(State& state, Player player);

   void spend_mana(State& state, Player player, size_t cost, bool spell_mana);
   // inline methods
   inline void shuffle_card_into_deck(State& state, const sptr< Card >& card, Player player)
   {
      auto& deck = m_state->deck(player);
      std::uniform_int_distribution< size_t > dist(0, deck.size());
      auto pos = deck.begin() + static_cast< long >(dist(rng::engine()));
      deck.insert(pos, card);
   }

   inline void obliterate(State& state, const sptr< Card >& card)
   {
      uncover_card(card);
      _remove(card);
   }
   static inline void uncover_card(State& state, const sptr< Card >& card)
   {
      card->mutables().hidden = false;
   }

  protected:
   State* m_state;

   void _move_units(
      State& state, const std::vector< size_t >& positions, Player player, bool to_bf);
   void _move_units_opp(
      State& state, const std::map< size_t, size_t >& positions, Player player, bool to_bf);
   bool _move_spell(State& state, const sptr< Spell >& spell, bool to_stack);
   void _mulligan(
      State& state,
      const std::vector< sptr< Card > >& hand_blue,
      const std::vector< sptr< Card > >& hand_red);

   std::vector< sptr< Card > > _draw_n_cards(State& state, Player player, int n = 1);

   /*
    * Start the next round.
    * Does the following actions in order:
    * - Set flag plunder to false and attack token to the attacker.
    * - Increase managems
    * - Refill mana
    */
   void _start_round(State& state);
   void _end_round(State& state);

   void _cast_spellstack(State& state);

   void _activate_battlemode(Player attack_player);
   void _deactivate_battlemode(State& state);

   void _remove(State& state, const sptr< Card >& card);

   void _resolve_battle(State& state);
   void _resolve_spell_stack(State& state, bool burst);

   void _check_enlightenment(State& state, Player player);
   void _play_event_triggers(State& state, const sptr< Card >& card, const Player& player);
   void _copy_grants(
      State& state,
      const std::vector< sptr< Grant > >& grants,
      const std::shared_ptr< Unit >& unit);
};

class ActiveMode: public LogicMode {
   void handle(State& state, const sptr< Action >& action) override;
   bool is_valid(State& state, const sptr< Action >& action) override;
};
class PassiveMode: public LogicMode {
   void handle(State& state, const sptr< Action >& action) override;
   bool is_valid(State& state, const sptr< Action >& action) override;
};
class AttackMode: public LogicMode {
   void handle(State& state, const sptr< Action >& action) override;
   bool is_valid(State& state, const sptr< Action >& action) override;
};
class BlockMode: public LogicMode {
   void handle(State& state, const sptr< Action >& action) override;
   bool is_valid(State& state, const sptr< Action >& action) override;
};
class BattleMode: public LogicMode {
   void handle(State& state, const sptr< Action >& action) override;
   bool is_valid(State& state, const sptr< Action >& action) override;
};
class TargetMode: public LogicMode {
   void handle(State& state, const sptr< Action >& action) override;
   bool is_valid(State& state, const sptr< Action >& action) override;
};
class MulliganMode: public LogicMode {
   void handle(State& state, const sptr< Action >& action) override;
   bool is_valid(State& state, const sptr< Action >& action) override;
};

#endif  // LORAINE_LOGIC_H
