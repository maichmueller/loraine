
#include "cards/types/unit.h"

#include "core/gamemode.h"

bool Unit::_check_play_condition(const State &state) const
{
   return state.player(mutables().owner).mana()->common >= mana_cost();
}
void Unit::add_power(long amount, bool permanent)
{
   if(permanent) {
      m_unit_mutables.power_base += amount;
   } else {
      m_unit_mutables.power_delta += amount;
   }
}
void Unit::add_health(long amount, bool permanent)
{
   if(permanent) {
      m_unit_mutables.health_base += amount;

   } else {
      m_unit_mutables.health_delta += amount;
   }
}
void Unit::health(size_t health)
{
   m_unit_mutables.health_delta = static_cast< decltype(m_unit_mutables.health_delta) >(
      health - m_unit_mutables.health_base);
}
void Unit::power(size_t power, bool as_delta)
{
   if(as_delta) {
      m_unit_mutables.power_delta = static_cast< decltype(m_unit_mutables.power_delta) >(
         power - m_unit_mutables.power_base);
   } else {
      m_unit_mutables.power_base = power;
      m_unit_mutables.power_delta = 0;
   }
}
