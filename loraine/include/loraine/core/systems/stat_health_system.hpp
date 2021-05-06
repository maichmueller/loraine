
#ifndef LORAINE_STAT_HEALTH_SYSTEM_HPP
#define LORAINE_STAT_HEALTH_SYSTEM_HPP

#include "loraine/core/components.hpp"
#include "loraine/core/systems/system.hpp"

class HealthStatSystem: public ILogicSystem {
   HealthStatSystem() = default;

   uint damage(entt::entity unit, entt::entity source, uint amount)
   {
      auto damage = m_registry->get< Damager >(unit).executor(*m_registry, unit, source, amount);
      m_registry->patch< HealthStat >(unit, update);
      return damage;
   }

   uint heal(entt::entity unit, entt::entity source, uint amount)
   {
      auto healed = m_registry->get< Healer >(unit).executor(*m_registry, unit, source, amount);
      m_registry->patch< HealthStat >(unit, update);
      return healed;
   }

  private:
   static void update(HealthStat& comp)
   {
      comp.health_cache = std::max(
         0L, comp.health_ref + comp.health_delta + comp.health_delta_temp + comp.damage);
   }

};
#endif  // LORAINE_STAT_HEALTH_SYSTEM_HPP
