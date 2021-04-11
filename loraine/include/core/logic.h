

#ifndef LORAINE_LOGIC_H
#define LORAINE_LOGIC_H

#include <events/lor_events/construction.h>
#include <grants/grant.h>

#include <array>

#include "action_invoker.h"
#include "events/event_subscriber.h"
#include "events/lor_events/event_labels.h"

// forward declare
class GameState;

class Logic: public Cloneable< Logic > {
  public:
   explicit Logic(uptr< ActionInvokerBase > act_invoker = std::make_unique< MulliganModeInvoker >())
       : m_action_invoker(std::move(act_invoker))
   {
      m_action_invoker->logic(this);
   };

   Logic(const Logic& other);
   Logic(Logic&& l) noexcept = default;
   Logic& operator=(Logic&& rhs) = default;
   Logic& operator=(const Logic& rhs) = delete;
   ~Logic() override = default;

   void state(GameState& state) { m_state = &state; }
   auto state() { return m_state; }
   [[nodiscard]] auto state() const { return m_state; }

   // Beginning of LoR logic implementations

   ActionInvokerBase& action_invoker() { return *m_action_invoker; }
   ActionInvokerBase& action_invoker() const { return *m_action_invoker; }

   [[nodiscard]] bool in_combat() const
   {
      return m_action_invoker->label() == ActionInvokerBase::Label::COMBAT
             || (m_action_invoker->label() != ActionInvokerBase::Label::DEFAULT
                 && m_prev_action_invoker->label() == ActionInvokerBase::Label::COMBAT);
   };

   void request_action() const;
   bool invoke_actions();

   Status step();

   // e.g. FIORA's win condition or Star Spring's.
   inline void external_win_trigger(Team team, bool nexus = true)
   {
      _set_status(Status::win(team, nexus));
   };

   void draw_card(Team team);

   void play_event_triggers(const sptr< Card >& card);

   void place_in_camp(const sptr< FieldCard >& card, const std::optional< size_t >& replaces);

   void init_attack(Team team);
   void init_block(Team team);
   void resolve();
   void unsubscribe_effects(const sptr< Card >& card);
   void subscribe_effects(const sptr< Card >& card, EffectBase::RegistrationTime registration_time);
   /**
    * Cast the given spells. This does not check whether the spell is also played.
    */
   void cast(bool burst);
   /**
    * Summon a specific common spell to either the camp or the battlefield.
    * @param: unit: shared_ptr<Unit>,
    *    the common to summon
    * @param: to_bf: boolean,
    *    to decide whether to summon it to the camp or battlefield.
    */
   void summon(const sptr< Unit >& unit, bool to_bf, bool is_play);
   /**
    * Increase/Decrease the managems of the given team.
    * @param: team: The team whose managems are changed.
    * @param: amount: (optional) the number of gems to add/remove. Defaults to 1.
    */
   void give_managems(Team team, long amount = 1);
   /**
    * Creates a spell as determined by the code.
    * @param team: shared_ptr<Card>,
    *    the team who will own the new spell
    * @param card_code: const char*,
    *    the character code determining which spell is created
    * @returns: shared_ptr<Card>,
    *    The created spell
    */
   sptr< Card > create(Team team, const char* card_code);
   /**
    * Copy the given spell (basic or exact).
    * @param card: shared_ptr<Card>,
    *    the spell to copy
    * @param exact_copy: boolean,
    *    decides whether to copy the spell with all grants attached. Defaults to `false`.
    * @returns: shared_ptr<Card>
    *    the spell copy.
    */
   sptr< Card > copy(const sptr< Card >& card, bool exact_copy = false);
   /** Recall the chosen spell.
    * @param recalled_card: shared_ptr<Card>,
    *   the spell to recall.
    * @param recaller: Team,
    *   the team who recalled the spell.
    */
   void recall(const sptr< Card >& recalled_card, Team recaller);

   /**
    * Let a common strike another.
    * @param unit_att: shared_ptr<Unit>,
    *   the striking common.
    * @param unit_def: shared_ptr<Unit>,
    *   the struck common.
    */
   long strike(const sptr< Unit >& unit_att, sptr< Unit >& unit_def);
   SymArr< long > strike_mutually(const sptr< Unit >& unit1, sptr< Unit >& unit2);
   long damage_unit(const sptr< Unit >& unit, const sptr< Card >& cause, long dmg);
   void heal(const sptr< Unit >& unit, const sptr< Card >& cause, size_t amount);
   /**
    * Let a unit strike another.
    * @param unit_att: shared_ptr<Unit>,
    *   the striking unit.
    * @param unit_def: shared_ptr<Unit>,
    *   the struck unit.
    */
   void strike_nexus(const sptr< Unit >& striking_unit, long dmg);
   void damage_nexus(const sptr< Card >& damaging_card, long dmg);
   void damage_nexus_simultan(const sptr< Card >& damaging_card, SymArr< long > dmgs);

   void start_game();

   template < typename NewInvokerType, typename... Args >
   inline void transition(Args&&... args);
   void restore_previous_invoker();

   void kill_unit(const sptr< Unit >& killed_unit, const sptr< Card >& cause);

   template < bool floating_mana = false >
   void clamp_mana();

   void pass();

   void reset_pass(Team team);
   void reset_pass();

   void refill_mana(Team team, bool normal_mana);

   template < GrantType grant_type, typename... Params >
   inline void grant(
      Team team,
      const sptr< Card >& bestowing_card,
      const sptr< Card >& card_to_bestow,
      Params&&... params)
   {
      //      auto grant = m_grant_factory[team]->grant< grant_type >(
      //         team, bestowing_card, card_to_bestow, std::forward< Params... >(params...));
      //      card_to_bestow->store_grant(grant);
      //      grant->apply(card_to_bestow);
   }

   /*
    * An api for triggering an event externally. Which m_subscribed_events is supposed to be
    * triggered needs to be known at compile time.
    */
   template < events::EventLabel event_type, typename... Params >
   inline void trigger_event(Params&&... params);

   template < Location range >
   std::vector< sptr< Targetable > > filter_targets(
      const Filter& filter,
      std::optional< Team > opt_team);

   void retreat_to_camp(Team team);
   void process_camp_queue(Team team);

   void spend_mana(const sptr< Card >& card);

   inline void obliterate(const sptr< Card >& card)
   {
      card->uncover();
      _remove(card);
   }
   Status check_status();

  private:
   /// The associated state ptr, to be set in a delayed manner after state construction
   /// The logic object's lifetime is bound to the GameState object, so there should be no
   /// SegFault problems.
   GameState* m_state = nullptr;
   /// the current action invoker for incoming actions
   std::unique_ptr< ActionInvokerBase > m_action_invoker;
   /// the previous action invoker for incoming actions
   std::unique_ptr< ActionInvokerBase > m_prev_action_invoker = nullptr;
   /// private logic helpers

   /// The member declarations
   void _start_round();
   void _end_round();

   void _trigger_daybreak_if(const sptr< Card >& card);
   void _trigger_nightfall_if(const sptr< Card >& card);

   void _remove(const sptr< Card >& card);

   void _check_enlightenment(Team team);
   void _copy_grants(
      const std::vector< sptr< Grant > >& grants,
      const std::shared_ptr< Unit >& unit);
   void _set_status(Status status);
};

#include "gamestate.h"

template < bool floating_mana >
void Logic::clamp_mana()
{
   for(int p = 0; p < n_teams; ++p) {
      Team team = static_cast< Team >(p);
      auto& mana = state()->player(team).mana();
      if(floating_mana) {
         auto clamped_mana = utils::clamp(mana.floating, 0UL, state()->config().MAX_FLOATING_MANA);
         mana.floating += clamped_mana;
      } else {
         auto clamped_mana = utils::clamp(mana.common, 0UL, mana.gems);
         mana.common += clamped_mana;
      }
   }
}

template < events::EventLabel event_label, typename... Params >
void Logic::trigger_event(Params&&... params)
{
   auto& event = m_state->event(event_label);
   event.detail< helpers::label_to_event_t< event_label > >().fire(
      *state(), std::forward< Params >(params)...);
}
//
// template < size_t E >
// void Logic::subscribe_effects_impl(
//   const sptr< Card >& card,
//   EffectBase::RegistrationTime registration_time)
//{
//   const auto elabel = static_cast< events::EventLabel >(E);
//   if(card->has_effect(elabel)) {
//      auto& event = m_state->event(elabel);
//      for(auto& effect : card->effects(elabel)) {
//         using EventType = helpers::label_to_event_t< elabel >;
//         using EffectType = helpers::eventlabel_to_effect_t< elabel >;
//         if(effect->registration_time() == registration_time) {
//            dynamic_cast< EffectType& >(*effect).template connect< EventType >(event);
//         }
//      }
//   }
//   if constexpr(E > 0) {
//      subscribe_effects_impl< E - 1 >(card, registration_time);
//   }
//}

// template < size_t E >
// void Logic::unsubscribe_effects_impl(const sptr< Card >& card)
//{
//    const auto elabel = static_cast< events::EventLabel >(E);
//    if(card->has_effect(elabel)) {
//       auto& event = m_state->event(elabel);
//       for(auto& effect : card->effects(elabel)) {
//          using EffectType = helpers::eventlabel_to_effect_t< elabel >;
//          dynamic_cast< EffectType& >(*effect).disconnect();
//       }
//    }
//    if constexpr(E > 0) {
//       unsubscribe_effects_impl< E - 1 >(card);
//    }
// }

template < typename NewInvokerType, typename... Args >
void Logic::transition(Args&&... args)
{
   // assert the chosen invoker type is any of the ones we have. It is not directly necessary to
   // do, but would improve the error message
   static_assert(
      utils::is_any_v<
         NewInvokerType,
         DefaultModeInvoker,
         CombatModeInvoker,
         MulliganModeInvoker,
         ReplacingModeInvoker,
         TargetModeInvoker >,
      "Given NewInvokerType is not one of the designated invokers.");

   // move current invoker into previous
   m_prev_action_invoker = std::move(m_action_invoker);
   m_action_invoker = std::make_unique< NewInvokerType >(this, std::forward< Args >(args)...);
}

#endif  // LORAINE_LOGIC_H
