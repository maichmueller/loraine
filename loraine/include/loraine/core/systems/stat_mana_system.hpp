

#ifndef LORAINE_STAT_MANA_SYSTEM_HPP
#define LORAINE_STAT_MANA_SYSTEM_HPP

#include "loraine/core/components.hpp"
#include "system.hpp"

class ManaStatSystem : public ILogicSystem {
   ManaStatSystem()  = default;

   uint change(entt::entity card, entt::entity source, long amount)
   {
      auto change = m_registry->get< ManaChanger >(card).executor(*m_registry, card, source, amount);
      m_registry->patch< ManaStat >(card, update);
      return change;
   }

  private:
   static inline void update(ManaStat& comp)
   {
      comp.cost_cache = std::max(0L, comp.cost_delta + comp.cost_ref);
   }
};

#endif  // LORAINE_STAT_MANA_SYSTEM_HPP
