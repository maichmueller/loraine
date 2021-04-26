

#ifndef LORAINE_STAT_MANA_SYSTEM_HPP
#define LORAINE_STAT_MANA_SYSTEM_HPP

#include "loraine/core/components.h"
#include "system.hpp"

class ManaStatSystem : public ILogicSystem {
   ManaStatSystem(entt::registry& registry) : ILogicSystem(registry) {}

   uint change(entt::entity entity, entt::entity source, long amount)
   {
      m_registry.get< ManaChanger >(entity).execute(m_registry, entity, source, amount);
   }

};

#endif  // LORAINE_STAT_MANA_SYSTEM_HPP
