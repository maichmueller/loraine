
#ifndef LORAINE_GAME_H
#define LORAINE_GAME_H

#include <event/event_listener.h>

#include "agent.h"
#include "cards/card.h"
#include "rulesets.h"
#include "state.h"
#include "types.h"

class Game {
  public:
   bool run_game();

   void incr_managems(Player player, size_t amount = 1);
   void decr_managems(Player player, size_t amount = 1);

   void play(const sptr< Spell >& spell);
   void play(const sptr< Unit >& unit, std::optional< size_t > replaces);
   void cast(const sptr< Spell >& spell);

   void summon(const sptr< Unit >& unit);
   void summon_to_battlefield(const sptr< Unit >& unit);
   void summon_exact_copy(const sptr< Unit >& unit);

   long int deal_damage_to_unit(
      const sptr< Card >& cause, sptr< Unit >& unit, const sptr< size_t >& damage);

   void nexus_strike(
      Player attacked_nexus,
      const sptr< size_t >& damage,
      const std::shared_ptr< Card >& responsible_card);

   long int strike(const std::shared_ptr< Unit >& unit_att, std::shared_ptr< Unit >& unit_def);

   void kill_unit(
      Player killer,
      const std::shared_ptr< Unit >& killed_unit,
      const std::optional< std::shared_ptr< Card > >& cause = {});

   std::vector< Target > filter_targets(
      Location range,
      const std::function< bool(const sptr< Card >&) >& filter,
      std::optional< Player > opt_player);

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
      auto uuid = card->get_uuid();
      m_event_listener.unregister_card(card);
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

   template < typename... Params >
   inline void grant(GrantType grant_type, Params&&... params)
   {
      switch(grant_type) {
         case Stats: {
            store_grant(std::make_shared< StatsGrant >(std::forward<Params...>(params...)));
         }
         case Mana: {
            store_grant(std::make_shared< ManaGrant >(std::forward<Params...>(params...)));
         }
         case Keyword: {
            store_grant(std::make_shared< KeywordGrant >(std::forward<Params...>(params...)));
         }
         case Effect: {
            store_grant(std::make_shared< EffectGrant >(std::forward<Params...>(params...)));
         }
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
   events::VariantEvent m_active_event;
   bool m_battle_mode = false;
   std::map< UUID, std::vector< sptr< Grant > > > m_grants_perm;
   std::map< UUID, std::vector< sptr< Grant > > > m_grants_temp;

   inline void _trigger_event(events::AnyEvent&& event)
   {
      m_active_event = std::move(event);
      m_event_listener.on_event(*this, m_active_event);
   }

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

   void _activate_battlemode(Player attack_player, std::vector< size_t > positions);

   void _deactivate_battlemode();

   void _resolve_battle();

   void _resolve_spell_stack();

   void _check_enlightenment(Player player);
};

#endif  // LORAINE_GAME_H
