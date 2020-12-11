
#ifndef LORAINE_GAME_H
#define LORAINE_GAME_H

#include <cards/algorithms.h>

#include <optional>

#include "agent.h"
#include "cards/card.h"
#include "cards/filter.h"
#include "cards/grant.h"
#include "event/event_listener.h"
#include "rulesets.h"
#include "state.h"
#include "types.h"

class Game {
  public:
   [[nodiscard]] const auto& get_active_event() const { return m_last_event; }
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

   void play_to_camp(const sptr< Card >& card, std::optional< size_t > replaces);
   void play_spells();
   void cast(const sptr< Spell >& spell);
   void cast_spellstack();

   void summon(const sptr< Unit >& unit);
   void summon_to_battlefield(const sptr< Unit >& unit);
   void summon_exact_copy(const sptr< Unit >& unit);

   void create_card(Player player, size_t card_id);
   void create_copy(Player player, const sptr< Card >& card, bool exact_copy = false);

   void recall(Player recaller, const sptr<Card>& recalled_card);

   void deal_damage_to_unit(
      const sptr< Card >& cause, const sptr< Unit >& unit, const sptr< long >& damage);

   void heal(Player player, const sptr< Unit >& unit, long amount);

   void nexus_strike(
      Player attacked_nexus, const sptr< long >& damage, const sptr< Card >& responsible_card);

   void strike(const std::shared_ptr< Unit >& unit_att, std::shared_ptr< Unit >& unit_def);

   void kill_unit(
      Player killer, const std::shared_ptr< Unit >& killed_unit, const sptr< Card >& cause = {});

   /*
    * An api for triggering an event mainly externally. Which event is supposed to be triggered
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
   static inline void uncover_card(const sptr< Card >& card) { card->set_flag_hidden(false); }
   //   void level_up_champion(sptr< Champion > champ);

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
         std::forward< Params... >(params...));
      card_to_bestow->store_grant(grant);
      grant->apply(card_to_bestow);
   }

  private:
   sptr< State > m_state;
   SymArr< sptr< Agent > > m_agents;
   events::EventListener m_event_listener;
   sptr< events::AnyEvent > m_last_event;
   SymArr< sptr< GrantFactory > > m_grant_factory;

   inline void _trigger_event(events::AnyEvent&& event)
   {
      m_last_event = std::make_shared< events::AnyEvent >(std::move(event));
      m_event_listener.on_event(*this, m_state->get_active_player(), *m_last_event);
   }

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
   void _activate_battlemode(Player attack_player);

   void _remove(const sptr< Card >& card);

   void _deactivate_battlemode();

   void _resolve_battle();
   void _resolve_spell_stack(bool burst);

   void _check_enlightenment(Player player);
   void _play_event_triggers(const sptr< Card >& card, const Player& player);
   void _copy_grants(
      const std::vector< sptr< Grant > >& grants, const std::shared_ptr< Unit >& unit);
};

template < events::EventType event_type, typename... Params >
constexpr inline void Game::trigger_event(Params... params)
{
   using namespace events;
   if constexpr(event_type == EventType::NONE) {
      _trigger_event(NoneEvent(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::ATTACK) {
      _trigger_event(AttackEvent(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::BEHOLD) {
      _trigger_event(BeholdEvent(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::CAPTURE) {
      _trigger_event(CaptureEvent(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::CAST) {
      _trigger_event(CastEvent(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::BLOCK) {
      _trigger_event(BlockEvent(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::DAYBREAK) {
      _trigger_event(DaybreakEvent(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::DIE) {
      _trigger_event(DieEvent(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::DISCARD) {
      _trigger_event(DiscardEvent(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::DRAW_CARD) {
      _trigger_event(DrawCardEvent(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::ENLIGHTENMENT) {
      _trigger_event(EnlightenmentEvent(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::GAIN_MANAGEM) {
      _trigger_event(GainManagemEvent(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::HEAL_UNIT) {
      _trigger_event(HealUnitEvent(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::LEVEL_UP) {
      _trigger_event(LevelUpEvent(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::NEXUS_STRIKE) {
      _trigger_event(NexusStrikeEvent(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::NIGHTFALL) {
      _trigger_event(NightfallEvent(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::PLAY) {
      _trigger_event(PlayEvent(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::RECALL) {
      _trigger_event(RecallEvent(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::ROUND_END) {
      _trigger_event(RoundEndEvent(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::ROUND_START) {
      _trigger_event(RoundStartEvent(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::SCOUT) {
      _trigger_event(ScoutEvent(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::STRIKE) {
      _trigger_event(StrikeEvent(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::STUN) {
      _trigger_event(StunEvent(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::SUMMON) {
      _trigger_event(SummonEvent(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::TARGET) {
      _trigger_event(TargetEvent(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::UNIT_TAKE_DAMAGE) {
      _trigger_event(UnitTakeDamageEvent(std::forward< Params... >(params...)));
   }
}

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
