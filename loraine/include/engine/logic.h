

#ifndef LORAINE_LOGIC_H
#define LORAINE_LOGIC_H

#include <grants/grant.h>

#include <array>

#include "action_handler.h"
#include "cards/card.h"
#include "engine/state.h"
#include "events/eventbase.h"
#include "events/event_labels.h"

// forward declare
class Action;

class Logic {
  public:
   explicit Logic(uptr< ActionHandler > act_handler = std::make_unique< MulliganModeHandler >())
       : m_action_handler(std::move(act_handler)){};

   Logic(const Logic& l) = delete;
   Logic(Logic&& l) noexcept = default;
   Logic& operator=(Logic&& rhs) = default;
   Logic& operator=(const Logic& rhs) = delete;
   virtual ~Logic() = default;

   void state(State& state) { m_state = &state; }
   auto state() { return m_state; }

   virtual void start_game(State& state);
   virtual void start_round(State& state);
   virtual void end_round(State& state);

   // Beginning of LoR logic implementations

   // methods to handle in the specific action handler mode of the state.
   void handle(const sptr< Action >& action) { m_action_handler->handle(action); }
   bool is_valid(const sptr< Action >& action) { return m_action_handler->is_valid(action); }

   /**
    * Play a fieldcard onto the board
    * @param card: shared_ptr<Card>,
    *    the card to play
    * @param replaces: optional<size_t>,
    *    index of the card in the camp it is supposed to replace.
    *    This only applies if the board is full.
    */
   void play(const sptr< FieldCard >& card, std::optional< size_t > replaces);
   /**
    * play all the spells that have been added to the spellstack
    */
   void play_spells();
   /**
    * Cast the given floating. This does not check whether the given floating is also played.
    * @param: spell: shared_ptr<Spell>,
    *    The floating to cast.
    */
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
    * Let a common strike another.
    * @param unit_att: shared_ptr<Unit>,
    *   the striking common.
    * @param unit_def: shared_ptr<Unit>,
    *   the struck common.
    */
   void nexus_strike(const sptr< Unit >& striking_unit);
   void heal(Team team, const sptr< Unit >& unit, long amount);
   void damage_unit(
      const sptr< Card >& cause, const sptr< Unit >& unit, const sptr< long >& damage);

   void kill_unit(Team killer, const sptr< Unit >& killed_unit, const sptr< Card >& cause = {});
   void damage_nexus(size_t amount, Team team);

   void heal_nexus(size_t amount, Team team);

   template < bool floating_mana = false >
   void clamp_mana();

   [[nodiscard]] auto is_enlightened(Team team) const;
   [[nodiscard]] bool round_ended() const;
   bool pass();

   void reset_pass(Team team);

   template < typename EventType, typename... Args >
   auto sort_event_subscribers(
      const std::vector< typename EventType::Subscribers* >& subs, Args... args)
      -> std::vector< typename EventType::Subscriber* >
   {
      return subs;
   }

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
   std::vector< Target > filter_targets_bf(const Filter& filter, std::optional< Team > opt_team);

   std::vector< Target > filter_targets_camp(const Filter& filter, std::optional< Team > opt_team);

   std::vector< Target > filter_targets_board(const Filter& filter, std::optional< Team > opt_team);

   std::vector< Target > filter_targets_hand(const Filter& filter, std::optional< Team > opt_team);

   std::vector< Target > filter_targets_deck(const Filter& filter, std::optional< Team > opt_team);

   std::vector< Target > filter_targets_everywhere(
      const Filter& filter, std::optional< Team > opt_team);

   /*
    * An api for triggering an event externally. Which events is supposed to be triggered
    * needs to be known at compile time.
    */
   template < events::EventLabel event_type, typename... Params >
   constexpr inline void trigger_event(Params... params);

   template < Location range >
   std::vector< Target > filter_targets(const Filter& filter, std::optional< Team > opt_team);

   void retreat_to_camp(Team team);
   void process_camp_queue(Team team);

   void draw_card(Team team);
   void spend_mana(Team team, size_t cost, bool floating_mana);

   // inline methods

   void shuffle_card_into_deck(const sptr< Card >& card, Team team, size_t top_n);
   inline void obliterate(const sptr< Card >& card)
   {
      uncover_card(card);
      _remove(card);
   }

   static inline void uncover_card(const sptr< Card >& card) { card->mutables().hidden = false; }
   bool check_status();
   [[nodiscard]] bool check_daybreak(Team team) const;
   [[nodiscard]] bool check_nightfall(Team team) const;

   /// The member declarations
  private:
   /// the associated state ptr, to be set in a delayed manner after state construction
   State* m_state = nullptr;
   /// the current action handler for incoming actions
   std::unique_ptr< ActionHandler > m_action_handler;

   /// private logic helpers
  private:
   void _move_units(const std::vector< size_t >& positions, Team team, bool to_bf);
   void _move_units_opp(const std::map< size_t, size_t >& positions, Team team, bool to_bf);

   bool _move_spell(const sptr< Spell >& spell, bool to_stack);

   void _mulligan(
      const std::vector< sptr< Card > >& hand_blue, const std::vector< sptr< Card > >& hand_red);

   std::vector< sptr< Card > > _draw_n_cards(Team team, int n = 1);

   void _cast_spellstack();
   void _activate_battlemode(Team attack_team);

   void _deactivate_battlemode();

   void _remove(const sptr< Card >& card);
   void _resolve_battle();

   void _resolve_spell_stack(bool burst);
   void _check_enlightenment(Team team);
   void _play_event_triggers(const sptr< Card >& card, const Team& team);
   void _copy_grants(
      const std::vector< sptr< Grant > >& grants, const std::shared_ptr< Unit >& unit);
};

template < Location range >
std::vector< Target > Logic::filter_targets(const Filter& filter, std::optional< Team > opt_team)
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
      if constexpr(floating_mana) {
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
template < events::EventLabel event_type, typename... Params >
constexpr void Logic::trigger_event(Params... params)
{
   m_state->events()[static_cast< size_t >(event_type)]->trigger
}

#endif  // LORAINE_LOGIC_H
