
#ifndef LORAINE_STAT_HEALTH_SYSTEM_HPP
#define LORAINE_STAT_HEALTH_SYSTEM_HPP

#include "loraine/core/components/all.h"
#include "loraine/core/systems/system.hpp"

class HealthStatSystem: public ILogicSystem {
   HealthStatSystem(entt::registry& registry) : ILogicSystem(registry) {}

   uint damage(entt::entity entity, entt::entity source, uint amount)
   {
      m_registry.get< Damager >(entity).execute(m_registry, entity, source, amount);
   }

   uint heal(entt::entity entity, entt::entity source, uint amount)
   {
      m_registry.get< Healer >(entity).execute(m_registry, entity, source, amount);
   }
};
#endif  // LORAINE_STAT_HEALTH_SYSTEM_HPP
