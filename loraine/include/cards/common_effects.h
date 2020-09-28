#ifndef LORAINE_COMMON_EFFECTS_H
#define LORAINE_COMMON_EFFECTS_H

#include <event/active_event.h>
#include <event/event.h>
#include <game.h>
#include <rulesets.h>

struct GiveManaGem {
   GiveManaGem(Game& game, Player player)
   {
      auto state = game.get_state();
      bool enlightened = state->is_enlightened(player);
      game.incr_managems(player);
      if(! enlightened && state->get_mana_gems(player) == MAX_MANA) {
         events::trigger(events::EnlightenmentEvent(player));
      }
   }
   void undo(Game& game, Player player) { game.decr_managems(player); }
};

struct GiveUnitStats {
   GiveUnitStats(
      Game& game, const sptr< Unit >& unit, long int power, size_t health = 0)
   : m_unit(unit), m_power(power), m_health(health)
   {
      unit->change_power(power);
      if(health != 0) {
         unit->add_health(health);
      }
   }

   void undo(Game& game) {
      m_unit->change_power(-m_power);
      if(m_health != 0) {
         if( m_unit->get_health() > m_unit->get_health_base()) {
            m_unit->set_health(m_unit->)
         }
      }
   }

   sptr<Unit> m_unit;
   long int m_power;
   size_t  m_health;
};

#endif  // LORAINE_COMMON_EFFECTS_H
