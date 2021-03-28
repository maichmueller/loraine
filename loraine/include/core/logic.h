

#ifndef LORAINE_LOGIC_H
#define LORAINE_LOGIC_H

#include <events/lor_events/construction.h>
#include <grants/grant.h>

#include <array>

#include "action_handler.h"
#include "events/event_labels.h"
#include "events/eventbase.h"

// forward declare
class State;

class Logic: public Cloneable< Logic > {
  public:
   explicit Logic(uptr< ActionHandlerBase > act_handler = std::make_unique< MulliganModeHandler >())
       : m_action_handler(std::move(act_handler))
   {
      m_action_handler->logic(this);
   };

   Logic(const Logic& other);
   Logic(Logic&& l) noexcept = default;
   Logic& operator=(Logic&& rhs) = default;
   Logic& operator=(const Logic& rhs) = delete;
   ~Logic() = default;

   void state(State& state) { m_state = &state; }
   auto state() { return m_state; }
   [[nodiscard]] auto state() const { return m_state; }

   // Beginning of LoR logic implementations

   // methods to handle in the specific action handler mode of the state.
   [[nodiscard]] inline bool is_valid(const actions::Action& action) const
   {
      return m_action_handler->is_valid(action);
   }
   [[nodiscard]] bool in_combat() const
   {
      auto combat_label = ActionHandlerBase::Label::COMBAT;
      return m_action_handler->label() == combat_label
             || m_prev_action_handler->label() == combat_label;
   };
   inline std::vector< actions::ActionLabel > accepted_action_labels()
   {
      return *(m_action_handler->accepted_actions());
   }
   inline std::vector< actions::Action > valid_actions(const State& state)
   {
      return m_action_handler->valid_actions(state);
   }
   [[nodiscard]] actions::Action request_action() const;

   Status step();

   void draw_card(Team team);
   /**
    * Play a fieldcard onto the board
    * @param card: shared_ptr<Card>,
    *    the card to play
    * @param replaces: optional<size_t>,
    *    index of the card in the camp it is supposed to m_replace.
    *    This only applies if the board is full.
    */
   void play(const sptr< FieldCard >& card, std::optional< size_t > replaces);
   /**
    * play all the spells that have been added to the spellstack
    */
   void play_spell(const sptr< Spell >& spell);

   /**
    * Cast the given floating. This does not check whether the given spell is also played.
    * @param: spell: shared_ptr<Spell>,
    *    The spell to cast.
    */
   template <
      typename Container,
      typename = std::enable_if_t<
         std::is_same_v< sptr< Spell >, typename std::decay_t< Container >::value_type > > >
   void cast(Container&& spells_vec);
   void cast(const sptr< Spell >& spell);
   /**
    * Summon a specific common card to either the camp or the battlefield.
    * @param: unit: shared_ptr<Unit>,
    *    the common to summon
    * @param: to_bf: boolean,
    *    to decide whether to summon it to the camp or battlefield.
    */
   void summon(const sptr< Unit >& unit, bool to_bf);
   /**
    * Increase/Decrease the managems of the given team.
    * @param: team: The team whose managems are changed.
    * @param: amount: (optional) the number of gems to add/remove. Defaults to 1.
    */
   void give_managems(Team team, long amount = 1);
   /**
    * Creates a card as determined by the code.
    * @param team: shared_ptr<Card>,
    *    the team who will own the new card
    * @param card_code: const char*,
    *    the character code determining which card is created
    * @returns: shared_ptr<Card>,
    *    The created card
    */
   sptr< Card > create(Team team, const char* card_code);
   /**
    * Copy the given card (basic or exact).
    * @param card: shared_ptr<Card>,
    *    the card to copy
    * @param exact_copy: boolean,
    *    decides whether to copy the card with all grants attached. Defaults to `false`.
    * @returns: shared_ptr<Card>
    *    the card copy.
    */
   sptr< Card > copy(const sptr< Card >& card, bool exact_copy = false);
   /** Recall the chosen card.
    * @param recalled_card: shared_ptr<Card>,
    *   the card to recall.
    * @param recaller: Team,
    *   the team who recalled the card.
    */
   void recall(const sptr< Card >& recalled_card, Team recaller);

   /**
    * Let a common strike another.
    * @param unit_att: shared_ptr<Unit>,
    *   the striking common.
    * @param unit_def: shared_ptr<Unit>,
    *   the struck common.
    */
   void strike(const sptr< Unit >& unit_att, sptr< Unit >& unit_def);
   /**
    * Let a unit strike another.
    * @param unit_att: shared_ptr<Unit>,
    *   the striking unit.
    * @param unit_def: shared_ptr<Unit>,
    *   the struck unit.
    */
   void nexus_strike(const sptr< Unit >& striking_unit);
   void heal(Team team, const sptr< Unit >& unit, long amount);

   void
   damage_unit(const sptr< Card >& cause, const sptr< Unit >& unit, const sptr< long >& damage);
   void start_game();

   template < typename NewHandlerType, typename... Args >
   inline void transition_action_handler(Args&&... args)
   {
      // assert the chosen handler type is any of the ones we have. It is not directly necessary to
      // do, but would improve the error message
      static_assert(
         utils::is_any_v<
            NewHandlerType,
            DefaultModeHandler,
            CombatModeHandler,
            MulliganModeHandler,
            TargetModeHandler >,
         "Given NewHandlerType is not one of the designated handlers.");

      // move current handler into previous
      m_prev_action_handler = std::move(m_action_handler);

      // assign new handler
      if constexpr(std::is_same_v< NewHandlerType, DefaultModeHandler >) {
         m_action_handler = std::make_unique< DefaultModeHandler >(
            this, std::forward< Args >(args)...);
      } else if constexpr(std::is_same_v< NewHandlerType, CombatModeHandler >) {
         m_action_handler = std::make_unique< CombatModeHandler >(
            this, std::forward< Args >(args)...);
      } else if constexpr(std::is_same_v< NewHandlerType, MulliganModeHandler >) {
         m_action_handler = std::make_unique< MulliganModeHandler >(
            this, std::forward< Args >(args)...);
      } else {
         m_action_handler = std::make_unique< TargetModeHandler >(
            this, std::forward< Args >(args)...);
      }
   }
   inline void restore_previous_handler()
   {
      utils::throw_if_no_value(
         m_prev_action_handler, "Previous action handler pointer holds no value.");
      m_action_handler = std::move(m_prev_action_handler);
      m_prev_action_handler = nullptr;
   }

   void kill_unit(Team killer, const sptr< Unit >& killed_unit, const sptr< Card >& cause = {});
   void damage_nexus(size_t amount, Team team);

   void heal_nexus(size_t amount, Team team);

   template < bool floating_mana = false >
   void clamp_mana();

   [[nodiscard]] bool round_ended() const;
   bool pass();

   void reset_pass(Team team);
   void reset_pass();

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
    * The optional team parameter decides whose cards are to be filtered. If
    * left as empty, then both teams' cards are filtered
    */
   std::vector< sptr< Targetable > > filter_targets_bf(
      const Filter& filter,
      std::optional< Team > opt_team);

   std::vector< sptr< Targetable > > filter_targets_camp(
      const Filter& filter,
      std::optional< Team > opt_team);

   std::vector< sptr< Targetable > > filter_targets_board(
      const Filter& filter,
      std::optional< Team > opt_team);

   std::vector< sptr< Targetable > > filter_targets_hand(
      const Filter& filter,
      std::optional< Team > opt_team);

   std::vector< sptr< Targetable > > filter_targets_deck(
      const Filter& filter,
      std::optional< Team > opt_team);

   std::vector< sptr< Targetable > > filter_targets_everywhere(
      const Filter& filter,
      std::optional< Team > opt_team);

   /*
    * An api for triggering an event externally. Which events is supposed to be triggered
    * needs to be known at compile time.
    */
   template < events::EventLabel event_type, typename... Params >
   inline void trigger_event(Params&&... params);

   template < Location range >
   std::vector< sptr< Targetable > > filter_targets(
      const Filter& filter,
      std::optional< Team > opt_team);

   void retreat_to_camp(Team team);
   void process_camp_queue(Team team);

   void spend_mana(const sptr<Card>& card);
   // inline methods

   void shuffle_card_into_deck(const sptr< Card >& card, Team team, size_t top_n);

   inline void obliterate(const sptr< Card >& card)
   {
      card->uncover();
      _remove(card);
   }
   Status check_status();

  private:

   /// The associated state ptr, to be set in a delayed manner after state construction
   /// The logic object's lifetime is bound to the State object, so there should be no
   /// SegFault problems.
   State* m_state = nullptr;
   /// the current action handler for incoming actions
   std::unique_ptr< ActionHandlerBase > m_action_handler;
   /// the previous action handler for incoming actions
   std::unique_ptr< ActionHandlerBase > m_prev_action_handler = nullptr;
   /// private logic helpers
   void _resolve_battle();
   void _resolve_spell_stack(bool burst);

   /// The member declarations
   void _start_round();
   void _end_round();
   std::vector< sptr< Card > > _draw_n_cards(Team team, int n = 1);

   void _cast_spellstack();
   void _activate_battlemode(Team attack_team);

   void _trigger_daybreak_if(const sptr<Card>& card);
   void _trigger_nightfall_if(const sptr<Card>& card);

   void _deactivate_battlemode();
   void _remove(const sptr< Card >& card);

   void _check_enlightenment(Team team);
   void _play_event_triggers(const sptr< Card >& card, const Team& team);
   void _copy_grants(
      const std::vector< sptr< Grant > >& grants,
      const std::shared_ptr< Unit >& unit);
   void refill_mana(Team team, bool normal_mana);
   void _set_status(Status status);
};

template < Location range >
std::vector< sptr< Targetable > > Logic::filter_targets(
   const Filter& filter,
   std::optional< Team > opt_team)
{
   if constexpr(range == Location::BATTLEFIELD) {
      return filter_targets_bf(filter, opt_team);
   } else if constexpr(range == Location::CAMP) {
      return filter_targets_camp(filter, opt_team);
   } else if constexpr(range == Location::BOARD) {
      return filter_targets_board(filter, opt_team);
   } else if constexpr(range == Location::HAND) {
      return filter_targets_hand(filter, opt_team);
   } else if constexpr(range == Location::DECK) {
      return filter_targets_deck(filter, opt_team);
   } else {
      return filter_targets_everywhere(filter, opt_team);
   }
}

#include "state.h"

template < bool floating_mana >
void Logic::clamp_mana()
{
   for(int p = 0; p < n_teams; ++p) {
      Team team = static_cast< Team >(p);
      if(floating_mana) {
         auto clamped_floating_mana = std::max(
            std::min(
               static_cast< size_t >(state()->config().MAX_FLOATING_MANA),
               state()->player(team).mana()->floating),
            size_t(0));
         state()->player(team).mana()->floating += clamped_floating_mana;
      } else {
         auto clamped_mana = std::max(
            std::min(
               std::min(
                  // MIN(max mana possible, current managems of team)
                  state()->config().MAX_MANA,
                  state()->player(team).mana()->gems),
               // MIN(previous val, current mana of team)
               state()->player(team).mana()->common),
            size_t(0));
         state()->player(team).mana()->common += clamped_mana;
      }
   }
}

template < events::EventLabel event_label, typename... Params >
void Logic::trigger_event(Params&&... params)
{
   auto& event = m_state->events()->at(static_cast< size_t >(event_label));
   // the event_label is used as index pointer to the actual type inside the LOREvent variant.
   // we need to infer this event_type and then call its trigger method.
   using targeted_event_t = std::invoke_result_t< decltype(&events::create_event< event_label >) >;
   std::get< targeted_event_t >(event).trigger(*state(), std::forward< Params >(params)...);
}

template < typename Container, typename >
void Logic::cast(Container&& spells_vec)
{
   for(const auto& spell : spells_vec) {
      trigger_event< events::EventLabel::CAST >(spell->mutables().owner, spell);
   }
}

#endif  // LORAINE_LOGIC_H
