
#ifndef LORAINE_GAME_H
#define LORAINE_GAME_H

#include <event/event_listener.h>

#include "agent.h"
#include "cards/card.h"
#include "rulesets.h"
#include "state.h"
#include "types.h"

class Game {
   sptr< State > m_state;
   SymArr< sptr< Agent > > m_agents;
   events::EventListener m_event_listener;
   events::VariantEvent m_active_event;

   bool m_battle_mode = false;

   void _mulligan(
      const std::vector< sptr< Card > >& hand_blue,
      const std::vector< sptr< Card > >& hand_red);
   std::vector< sptr< Card > > _draw_initial_hand(PLAYER player);
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

   void _activate_battlemode(
      PLAYER attack_player, std::vector< size_t > positions);

   void _resolve_battle();

   void _resolve_spell_stack();

   void _nexus_strike(
      PLAYER attacked_nexus,
      PLAYER attacking_player,
      const sptr< size_t >& damage,
      const std::shared_ptr< Card >& responsible_card,
      bool strike_during_combat);

   long int _strike_unit(
      PLAYER attacker,
      PLAYER defender,
      const std::shared_ptr< Unit >& unit_att,
      std::shared_ptr< Unit >& unit_def);

   void _kill_unit(
      PLAYER killer,
      const std::shared_ptr< Unit >& killed_unit,
      const std::shared_ptr< Card >& killing_card);

   long int _deal_damage_to_unit(
      PLAYER unit_owner,
      std::shared_ptr< Unit >& unit,
      const sptr< size_t >& damage);

   void _check_enlightenment(PLAYER player);

  public:
   bool run_game();
   void incr_managems(PLAYER player, size_t amount = 1);
   void decr_managems(PLAYER player, size_t amount = 1);

   void play(sptr< Unit > unit);
   void cast(sptr< Spell > spell);
   void summon(sptr< Unit > unit);
   void summon_exact_copy(sptr< Unit > unit);

   inline void lifesteal(const sptr<Unit>& unit, size_t amount) {
      m_state->heal_nexus(amount, unit->get_owner());
   }

   inline void set(events::AnyEvent&& event)
   {
      m_active_event = std::move(event);

      m_event_listener.on_event(m_active_event);
   }
   inline void trigger_event(events::AnyEvent&& event) {
      m_active_event = std::move(event);
   }

   [[nodiscard]] const auto& get_active_event() const { return m_active_event; }
   [[nodiscard]] auto get_state() const { return m_state; }
};

#endif  // LORAINE_GAME_H
