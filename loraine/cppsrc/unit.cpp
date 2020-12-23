
#include "cards/card_types/unit.h"
#include "game.h"

bool Unit::_check_play_condition(const Game& game) const
{
   return game.get_state()->get_mana(get_mutable_attrs().owner) >= get_mana_cost();
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
void Unit::set_health(size_t health)
{
   m_unit_mutables.health_delta = static_cast< decltype(m_unit_mutables.health_delta) >(health - m_unit_mutables.health_base);
}
void Unit::set_power(size_t power, bool as_delta)
{
   if(as_delta) {
      m_unit_mutables.power_delta = static_cast< decltype(m_unit_mutables.power_delta) >(power - m_unit_mutables.power_base);
   } else {
      m_unit_mutables.power_base = power;
      m_unit_mutables.power_delta = 0;
   }
}
