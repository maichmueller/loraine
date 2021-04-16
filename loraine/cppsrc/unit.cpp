
#include "loraine/cards/types/unit.h"

#include "loraine/core/gamemode.h"

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
long Unit::take_damage(const sptr< Card >& damaging_card, long amount)
{
   if(has_keyword(Keyword::TOUGH)) {
      --amount;
   }
   for(auto& dmg_modifier : m_unit_mutables.dmg_modifiers) {
      dmg_modifier(*damaging_card, amount);
   }
   auto health_before = health();
   m_unit_mutables.damage += amount;
   // the damage taken by the unit is returned: health_before - health_after
   return health_before - health();
}
void Unit::kill(const sptr< Card >& cause)
{
   if(m_unit_mutables.kill_func) {
      m_unit_mutables.kill_func(cause);
   } else {
      m_unit_mutables.alive = false;
   }
}
size_t Unit::heal(size_t amount)
{
   auto before = m_unit_mutables.damage;
   m_unit_mutables.damage -= std::min(m_unit_mutables.damage, amount);
   return before - m_unit_mutables.damage;
}
long Unit::health_raw() const
{
   long health_base = static_cast< long int >(m_unit_mutables.health_base);
   long damage = static_cast< long >(m_unit_mutables.damage);
   return health_base + m_unit_mutables.health_delta - damage;
}

long Unit::power_raw() const
{
   return m_unit_mutables.power_base + static_cast< size_t >(m_unit_mutables.power_delta);
}
