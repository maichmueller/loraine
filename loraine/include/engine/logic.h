

#ifndef LORAINE_LOGIC_H
#define LORAINE_LOGIC_H

#include <grants/grant.h>

#include <array>

#include "events/event.h"
#include "events/event_types.h"

// forward declare
class State;
class Action;

class Logic {
  public:
   Logic() : m_state(nullptr) {}
   explicit Logic(State& state) : m_state(&state) {}

   Logic(const Logic& l) = default;
   Logic(Logic&& l) noexcept = default;
   Logic& operator=(Logic&& rhs) = default;
   Logic& operator=(const Logic& rhs) = default;
   virtual ~Logic() = default;

   void state(State& state) { m_state = &state; }
   auto state() { return m_state; }

   // methods to handle in the specific logic mode on the state.

   virtual void handle(const sptr< Action >& action) = 0;
   virtual bool is_valid(const sptr< Action >& action) = 0;

   // Beginning of LoR logic implementations

   /**
    * Play a fieldcard onto the board
    *
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
    * Cast the given spell. This does not check whether the given spell is also played.
    *
    * @param: spell: shared_ptr<Spell>,
    *    The spell to cast.
    *
    */
   void cast(const sptr< Spell >& spell);
   /**
    * Summon a specific unit card to either the camp or the battlefield.
    *
    * @param: unit: shared_ptr<Unit>,
    *    the unit to summon
    * @param: to_bf: boolean,
    *    to decide whether to summon it to the camp or battlefield.
    */
   void summon(const sptr< Unit >& unit, bool to_bf);
   /**
    * Increase/Decrease the managems of the given player.
    *
    * @param: player: The player whose managems are changed.
    * @param: amount: (optional) the number of gems to add/remove. Defaults to 1.
    */
   void give_managems(Player player, long amount = 1);
   /**
    * Creates a card as determined by the code.
    *
    * @param player: shared_ptr<Card>,
    *    the player who will own the new card
    * @param card_code: const char*,
    *    the character code determining which card is created
    * @returns: shared_ptr<Card>,
    *    The created card
    */
   sptr< Card > create(Player player, const char* card_code);
   /**
    * Copy a given card
    *
    * @param card: shared_ptr<Card>,
    *    the card to copy
    * @param exact_copy: boolean,
    *    decides whether to copy the card with all grants attached. Defaults to `false`.
    * @returns: shared_ptr<Card>
    *    the card copy.
    */
   sptr< Card > copy(const sptr< Card >& card, bool exact_copy = false);
   /** Recall the chosen card.
    *
    * @param recalled_card
    */
   void recall(const sptr< Card >& recalled_card, Player recaller);

   void strike(const sptr< Unit >& unit_att, sptr< Unit >& unit_def);
   void nexus_strike(Player attacked_nexus, const sptr< Unit >& striking_unit);

   void heal(Player player, const sptr< Unit >& unit, long amount);
   void damage_unit(
      const sptr< Card >& cause, const sptr< Unit >& unit, const sptr< long >& damage);
   void kill_unit(Player killer, const sptr< Unit >& killed_unit, const sptr< Card >& cause = {});

   void damage_nexus(size_t amount, Player player);
   void heal_nexus(size_t amount, Player player);

   template < bool spell_mana = false >
   void clamp_mana();

   [[nodiscard]] auto is_enlightened(Player player) const;

   [[nodiscard]] bool round_ended() const;
   bool pass();
   void reset_pass(Player player);

   template < GrantType grant_type, typename... Params >
   inline void grant(
      Player player,
      const sptr< Card >& bestowing_card,
      const sptr< Card >& card_to_bestow,
      Params&&... params)
   {
      //      auto grant = m_grant_factory[player]->grant< grant_type >(
      //         player, bestowing_card, card_to_bestow, std::forward< Params... >(params...));
      //      card_to_bestow->store_grant(grant);
      //      grant->apply(card_to_bestow);
   }

   /*
    * The optional player parameter decides whose cards are to be filtered. If
    * left as empty, then both players' cards are filtered
    */
   std::vector< Target > filter_targets_bf(
      const Filter& filter, std::optional< Player > opt_player);

   std::vector< Target > filter_targets_camp(
      const Filter& filter, std::optional< Player > opt_player);

   std::vector< Target > filter_targets_board(
      const Filter& filter, std::optional< Player > opt_player);

   std::vector< Target > filter_targets_hand(
      const Filter& filter, std::optional< Player > opt_player);

   std::vector< Target > filter_targets_deck(
      const Filter& filter, std::optional< Player > opt_player);

   std::vector< Target > filter_targets_everywhere(
      const Filter& filter, std::optional< Player > opt_player);

   /*
    * An api for triggering an events mainly externally. Which events is supposed to be triggered
    * needs to be known at compile time.
    */
   template < events::EventType event_type, typename... Params >
   constexpr inline void trigger_event(event_type, Params... params);

   template < Location range >
   std::vector< Target > filter_targets(const Filter& filter, std::optional< Player > opt_player);

   void retreat_to_camp(Player player);

   void process_camp_queue(Player player);
   void draw_card(Player player);

   void spend_mana(Player player, size_t cost, bool spell_mana);
   // inline methods

   void shuffle_card_into_deck(const sptr< Card >& card, Player player);

   inline void obliterate(const sptr< Card >& card)
   {
      uncover_card(card);
      _remove(card);
   }
   static inline void uncover_card(const sptr< Card >& card) { card->mutables().hidden = false; }

   [[nodiscard]] bool check_daybreak(Player player) const;
   [[nodiscard]] bool check_nightfall(Player player) const;

  private:
   State* m_state;

   void _move_units(const std::vector< size_t >& positions, Player player, bool to_bf);
   void _move_units_opp(const std::map< size_t, size_t >& positions, Player player, bool to_bf);
   bool _move_spell(const sptr< Spell >& spell, bool to_stack);
   void _mulligan(
      State& state,
      const std::vector< sptr< Card > >& hand_blue,
      const std::vector< sptr< Card > >& hand_red);

   std::vector< sptr< Card > > _draw_n_cards(Player player, int n = 1);

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

   void _remove(const sptr< Card >& card);

   void _resolve_battle(State& state);
   void _resolve_spell_stack(bool burst);

   void _check_enlightenment(Player player);
   void _play_event_triggers(const sptr< Card >& card, const Player& player);
   void _copy_grants(
      State& state,
      const std::vector< sptr< Grant > >& grants,
      const std::shared_ptr< Unit >& unit);
};

template < Location range >
std::vector< Target > Logic::filter_targets(
   const Filter& filter, std::optional< Player > opt_player)
{
   if constexpr(range == Location::BATTLEFIELD) {
      return filter_targets_bf(filter, opt_player);
   } else if constexpr(range == Location::CAMP) {
      return filter_targets_camp(filter, opt_player);
   } else if constexpr(range == Location::BOARD) {
      return filter_targets_board(filter, opt_player);
   } else if constexpr(range == Location::HAND) {
      return filter_targets_hand(filter, opt_player);
   } else if constexpr(range == Location::DECK) {
      return filter_targets_deck(filter, opt_player);
   } else {
      return filter_targets_everywhere(filter, opt_player);
   }
}

class ActiveModeLogic: public Logic {
   void handle(const sptr< Action >& action) override;
   bool is_valid(const sptr< Action >& action) override;
};
class PassiveModeLogic: public Logic {
   void handle(const sptr< Action >& action) override;
   bool is_valid(const sptr< Action >& action) override;
};
class AttackModeLogic: public Logic {
   void handle(const sptr< Action >& action) override;
   bool is_valid(const sptr< Action >& action) override;
};
class BlockModeLogic: public Logic {
   void handle(const sptr< Action >& action) override;
   bool is_valid(const sptr< Action >& action) override;
};
class CombatModeLogic: public Logic {
   void handle(const sptr< Action >& action) override;
   bool is_valid(const sptr< Action >& action) override;
};
class TargetModeLogic: public Logic {
   void handle(const sptr< Action >& action) override;
   bool is_valid(const sptr< Action >& action) override;
};
class MulliganModeLogic: public Logic {
   void handle(const sptr< Action >& action) override;
   bool is_valid(const sptr< Action >& action) override;
};

#include "state.h"

template < bool spell_mana >
void Logic::clamp_mana()
{
   for(int p = 0; p < n_players; ++p) {
      Player player = static_cast< Player >(p);
      if constexpr(spell_mana) {
         auto clamped_spell_mana = std::max(
            std::min(
               static_cast< size_t >(state()->config().MAX_SPELL_MANA),
               state()->spell_mana(player)),
            size_t(0));
         state()->spell_mana(clamped_spell_mana, player);
      } else {
         auto clamped_mana = std::max(
            std::min(
               std::min(
                  // MIN(max mana possible, current managems of player)
                  state()->config().MAX_MANA,
                  state()->managems(player)),
               // MIN(previous val, current mana of player)
               state()->mana(player)),
            size_t(0));
         state()->mana(clamped_mana, player);
      }
   }
}

#endif  // LORAINE_LOGIC_H
