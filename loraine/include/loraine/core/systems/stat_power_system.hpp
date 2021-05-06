
#ifndef LORAINE_STAT_POWER_SYSTEM_HPP
#define LORAINE_STAT_POWER_SYSTEM_HPP

#include "loraine/core/components.hpp"
#include "system.hpp"

class PowerStatSystem: public ILogicSystem {
   PowerStatSystem() = default;
   uint buff(entt::entity unit, entt::entity source, long amount)
   {
      auto buff = m_registry->get< Buffer >(unit).executor(*m_registry, unit, source, amount);
      m_registry->patch< PowerStat >(unit, update);
      return buff;
   }

  private:
   static inline void update(PowerStat& comp)
   {
      comp.power_cache = std::max(0L, comp.power_delta + comp.power_ref);
   }
};
#endif  // LORAINE_STAT_POWER_SYSTEM_HPP
