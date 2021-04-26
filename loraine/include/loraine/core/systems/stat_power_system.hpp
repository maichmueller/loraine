
#ifndef LORAINE_STAT_POWER_SYSTEM_HPP
#define LORAINE_STAT_POWER_SYSTEM_HPP

#include "loraine/core/components.h"
#include "system.hpp"

class PowerStatSystem : public ILogicSystem {
   PowerStatSystem(entt::registry& registry) : ILogicSystem(registry) {}

   uint buff(entt::entity entity, entt::entity source, long amount)
   {
      m_registry.get< Buffer >(entity).execute(m_registry, entity, source, amount);
   }


};
#endif  // LORAINE_STAT_POWER_SYSTEM_HPP
