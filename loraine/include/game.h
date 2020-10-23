
#ifndef LORAINE_GAME_H
#define LORAINE_GAME_H

#include "agent.h"
#include "cards/card.h"
#include "event/event_listener.h"
#include "rulesets.h"
#include "state.h"
#include "types.h"

class Game {
  public:
   [[nodiscard]] inline auto& get_board() const { return m_board; }
   [[nodiscard]] inline auto& get_agent(Player player) const { return m_agents[player]; }

   [[nodiscard]] bool check_daybreak(Player player) const;
   [[nodiscard]] bool check_nightfall(Player player) const;

   bool run_game();

   void incr_managems(Player player, size_t amount = 1);
   void decr_managems(Player player, size_t amount = 1);

   void play(const sptr< Spell >& spell);
   void play(const sptr< Unit >& unit, std::optional< size_t > replaces);
   void cast(const sptr< Spell >& spell);

   void summon(const sptr< Unit >& unit);
   void summon_to_battlefield(const sptr< Unit >& unit);
   void summon_exact_copy(const sptr< Unit >& unit);

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
   std::vector< Target > filter_targets(
      const std::function< bool(const sptr< Card >&) >& filter, std::optional< Player > opt_player);

   [[nodiscard]] const auto& get_active_event() const { return m_active_event; }
   [[nodiscard]] auto get_state() const { return m_state; }
   [[nodiscard]] std::vector< sptr< Grant > > get_all_grants(const sptr< Card >& card) const;

   void retreat_to_camp(Player player);
   void process_camp_queue(Player player);

   void draw_card(Player player);
   void spend_mana(Player player, size_t cost, bool spell_mana);

   // inline methods
   inline void obliterate(const sptr< Card >& card)
   {
      uncover_card(card);
      remove(card);
   }
   static inline void uncover_card(const sptr< Card >& card) { card->set_flag_hidden(false); }
   inline void remove(const sptr< Card >& card)
   {
      m_event_listener.unregister_card(card);
      auto uuid = card->get_uuid();
      m_grants_perm.erase(uuid);
      m_grants_temp.erase(uuid);
   }

   //   void level_up_champion(sptr< Champion > champ);
   /*
    * The optional player parameter decides whose cards are to be filtered. If
    * left as empty, then both players' cards are filtered
    */
   std::vector< Target > filter_targets_bf(
      const std::function< bool(const sptr< Unit >&) >& filter, std::optional< Player > opt_player);

   std::vector< Target > filter_targets_camp(
      const std::function< bool(const sptr< Unit >&) >& filter, std::optional< Player > opt_player);

   std::vector< Target > filter_targets_board(
      const std::function< bool(const sptr< Unit >&) >& filter, std::optional< Player > opt_player);

   std::vector< Target > filter_targets_hand(
      const std::function< bool(const sptr< Card >&) >& filter, std::optional< Player > opt_player);

   std::vector< Target > filter_targets_deck(
      const std::function< bool(const sptr< Card >&) >& function,
      std::optional< Player > anOptional);

   std::vector< Target > filter_targets_everywhere(
      const std::function< bool(const sptr< Card >&) >& filter, std::optional< Player > opt_player);

   template < GrantType grant_type, typename... Params >
   inline void grant(Params&&... params)
   {
      if constexpr(grant_type == Stats) {
         store_grant(std::make_shared< StatsGrant >(std::forward< Params... >(params...)));
      } else if constexpr(grant_type == Mana) {
         store_grant(std::make_shared< ManaGrant >(std::forward< Params... >(params...)));
      } else if constexpr(grant_type == Keyword) {
         store_grant(std::make_shared< KeywordGrant >(std::forward< Params... >(params...)));
      } else if constexpr(grant_type == Effect) {
         store_grant(std::make_shared< EffectGrant >(std::forward< Params... >(params...)));
      }
   }

   inline void store_grant(const sptr< Grant >& grant)
   {
      if(grant->is_permanent()) {
         m_grants_perm[grant->get_bestowed_card()->get_uuid()].emplace_back(grant);
      } else {
         m_grants_temp[grant->get_bestowed_card()->get_uuid()].emplace_back(grant);
      }
   }

   inline void store_grant(const std::vector< sptr< Grant > >& grants)
   {
      for(const auto& grant : grants) {
         store_grant(grant);
      }
   }

   inline void copy_grant(const sptr< Grant >& grant, const sptr< Card >& card_to_bestow)
   {
      store_grant(grant->copy_on(card_to_bestow));
   }

   inline void copy_grant(
      const std::vector< sptr< Grant > >& grants, const sptr< Card >& card_to_bestow)
   {
      for(const auto& grant : grants) {
         store_grant(grant->copy_on(card_to_bestow));
      }
   }

  private:
   sptr< State > m_state;
   sptr< Board > m_board;
   SymArr< sptr< Agent > > m_agents;
   events::EventListener m_event_listener;
   sptr< events::AnyEvent > m_active_event;
   bool m_battle_mode = false;
   std::map< UUID, std::vector< sptr< Grant > > > m_grants_perm;
   std::map< UUID, std::vector< sptr< Grant > > > m_grants_temp;

   inline void _trigger_event(events::AnyEvent&& event)
   {
      m_active_event = std::make_shared< events::AnyEvent >(std::move(event));
      m_event_listener.on_event(*this, m_state->get_turn(), *m_active_event);
   }

   void _move_units(const std::vector< size_t >& positions, Player player, bool to_bf);
   void _move_units_opp(const std::map< size_t, size_t >& positions, Player player, bool to_bf);

   void _mulligan(
      const std::vector< sptr< Card > >& hand_blue, const std::vector< sptr< Card > >& hand_red);

   std::vector< sptr< Card > > _draw_initial_hand(Player player);
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

   void _deactivate_battlemode();

   void _resolve_battle();

   void _resolve_spell_stack(bool burst);

   void _check_enlightenment(Player player);
   void play_event_triggers(const sptr< Card >& card, const Player& player);
};

template < events::EventType event_type, typename... Params >
constexpr inline void Game::trigger_event(Params... params)
{
   using namespace events;
   if constexpr(event_type == EventType::NONE) {
      _trigger_event(std::make_shared< NoneEvent >(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::ATTACK) {
      _trigger_event(std::make_shared< AttackEvent >(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::BEHOLD) {
      _trigger_event(std::make_shared< BeholdEvent >(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::CAPTURE) {
      _trigger_event(std::make_shared< CaptureEvent >(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::CAST) {
      _trigger_event(std::make_shared< CastEvent >(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::BLOCK) {
      _trigger_event(std::make_shared< BlockEvent >(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::DAYBREAK) {
      _trigger_event(std::make_shared< DaybreakEvent >(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::DIE) {
      _trigger_event(std::make_shared< DieEvent >(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::DISCARD) {
      _trigger_event(std::make_shared< DiscardEvent >(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::DRAW_CARD) {
      _trigger_event(std::make_shared< DrawCardEvent >(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::ENLIGHTENMENT) {
      _trigger_event(std::make_shared< EnlightenmentEvent >(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::GAIN_MANAGEM) {
      _trigger_event(std::make_shared< GainManagemEvent >(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::HEAL_UNIT) {
      _trigger_event(std::make_shared< HealUnitEvent >(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::LEVEL_UP) {
      _trigger_event(std::make_shared< LevelUpEvent >(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::NEXUS_STRIKE) {
      _trigger_event(std::make_shared< NexusStrikeEvent >(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::NIGHTFALL) {
      _trigger_event(std::make_shared< NightfallEvent >(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::PLAY) {
      _trigger_event(std::make_shared< PlayEvent >(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::RECALL) {
      _trigger_event(std::make_shared< RecallEvent >(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::ROUND_END) {
      _trigger_event(std::make_shared< RoundEndEvent >(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::ROUND_START) {
      _trigger_event(std::make_shared< RoundStartEvent >(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::SCOUT) {
      _trigger_event(std::make_shared< ScoutEvent >(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::STRIKE) {
      _trigger_event(std::make_shared< StrikeEvent >(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::STUN) {
      _trigger_event(std::make_shared< StunEvent >(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::SUMMON) {
      _trigger_event(std::make_shared< SummonEvent >(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::TARGET) {
      _trigger_event(std::make_shared< TargetEvent >(std::forward< Params... >(params...)));
   } else if constexpr(event_type == EventType::UNIT_TAKE_DAMAGE) {
      _trigger_event(std::make_shared< UnitTakeDamageEvent >(std::forward< Params... >(params...)));
   }
}

template < Location range >
std::vector< Target > Game::filter_targets(
   const std::function< bool(const sptr< Card >&) >& filter, std::optional< Player > opt_player)
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
