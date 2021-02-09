
#ifndef LORAINE_GAME_H
#define LORAINE_GAME_H

#include <algorithms.h>

#include <optional>

#include "agent.h"
#include "cards/card.h"
#include "cards/filter.h"
#include "events/event_listener.h"
#include "grants/grant.h"
#include "grants/grantfactory.h"
#include "rulesets.h"
#include "state.h"
#include "types.h"

class Game {
  public:
   [[nodiscard]] auto get_state() const { return m_state; }
   [[nodiscard]] inline auto& get_agent(Player player) const { return m_agents[player]; }
   [[nodiscard]] inline auto& get_grantfactory(Player player) const
   {
      return m_grant_factory[player];
   }

   [[nodiscard]] bool check_daybreak(Player player) const;
   [[nodiscard]] bool check_nightfall(Player player) const;

   bool run_game();

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
   std::vector< Target > filter_targets(const Filter& filter, std::optional< Player > opt_player);

   void retreat_to_camp(Player player);

   void process_camp_queue(Player player);
   void draw_card(Player player);

   void spend_mana(Player player, size_t cost, bool spell_mana);
   // inline methods
   inline void shuffle_card_into_deck(const sptr< Card >& card, Player player)
   {
      auto& deck = m_state->get_deck(player);
      std::uniform_int_distribution< size_t > dist(0, deck.size());
      auto pos = deck.begin() + static_cast< long >(dist(rng::get_engine()));
      deck.insert(pos, card);
   }

   inline void obliterate(const sptr< Card >& card)
   {
      uncover_card(card);
      _remove(card);
   }
   static inline void uncover_card(const sptr< Card >& card) { card->get_mutable_attrs().hidden = false; }
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

   template < GrantType grant_type, typename... Params >
   inline void grant(
      Player player,
      const sptr< Card >& bestowing_card,
      const sptr< Card >& card_to_bestow,
      Params&&... params)
   {
      auto grant = m_grant_factory[player]->grant< grant_type >(
         player, bestowing_card, card_to_bestow, std::forward< Params... >(params...));
      card_to_bestow->store_grant(grant);
      grant->apply(card_to_bestow);
   }

  private:

   sptr< State > m_state;
   SymArr< sptr< Agent > > m_agents;
   events::EventListener m_event_listener;
   SymArr< sptr< GrantFactory > > m_grant_factory;

   void _move_units(const std::vector< size_t >& positions, Player player, bool to_bf);

   void _move_units_opp(const std::map< size_t, size_t >& positions, Player player, bool to_bf);
   bool _move_spell(const sptr< Spell >& spell, bool to_stack);
   void _mulligan(
      const std::vector< sptr< Card > >& hand_blue, const std::vector< sptr< Card > >& hand_red);

   std::vector< sptr< Card > > _draw_n_cards(Player player, int n = 1);

   bool _do_action(const sptr< AnyAction >& action);

   /*
    * Start the next round.
    * Does the following actions in order:
    * - Set flag plunder to false and attack token to the attacker.
    * - Increase managems
    * - Refill mana
    */
   void _start_round();
   void _end_round();

   void _cast_spellstack();

   void _activate_battlemode(Player attack_player);
   void _deactivate_battlemode();

   void _remove(const sptr< Card >& card);

   void _resolve_battle();
   void _resolve_spell_stack(bool burst);

   void _check_enlightenment(Player player);
   void _play_event_triggers(const sptr< Card >& card, const Player& player);
   void _copy_grants(
      const std::vector< sptr< Grant > >& grants, const std::shared_ptr< Unit >& unit);
};

template < Location range >
std::vector< Target > Game::filter_targets(const Filter& filter, std::optional< Player > opt_player)
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

#endif  // LORAINE_GAME_H
