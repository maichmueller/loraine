
#ifndef LORAINE_STAT_HEALTH_H
#define LORAINE_STAT_HEALTH_H

struct HealthStat {
   /// the health of the card on creation
   long health_ref;
   /// the actual health at the moment
   unsigned int health_cache;
   /// the change to the health cost of the card
   long health_delta = 0;
   /// the temporary change to the health of the card, which can reduce damage at round end
   long health_delta_temp = 0;
   /// the current damage taken
   unsigned int damage = 0;
};

namespace detail {

struct default_damager {
   static size_t
   operation(entt::registry& registry, entt::entity damaged_unit, entt::entity source, uint amount)
   {
      auto& comp = registry.get< HealthStat >(damaged_unit);
      auto effective_damage = std::min(comp.health_cache, amount);
      comp.damage += effective_damage;
      update(comp);
      return effective_damage;
   }

   static inline void update(HealthStat& comp)
   {
      comp.health_cache = std::max(
         0L, comp.health_ref + comp.health_delta + comp.health_delta_temp + comp.damage);
   }
};

struct default_healer {
   static size_t
   operation(entt::registry& registry, entt::entity healed_unit, entt::entity source, uint amount)
   {
      auto& comp = registry.get< HealthStat >(healed_unit);
      auto effective_healing = std::min(amount, comp.damage);
      comp.damage -= effective_healing;
      update(comp);
      return effective_healing;
   }

   static inline void update(HealthStat& comp)
   {
      comp.health_cache = std::max(
         0L, comp.health_ref + comp.health_delta + comp.health_delta_temp + comp.damage);
   }
};

}  // namespace detail

struct Damager {
   using FuncType = std::function< size_t(
      entt::registry& registry,
      entt::entity damaged_unit,
      entt::entity source,
      size_t amount) >;
   FuncType execute = &detail::default_damager::operation;
};

struct Healer {
   using FuncType = std::function< size_t(
      entt::registry& registry,
      entt::entity healed_unit,
      entt::entity source,
      size_t amount) >;
   FuncType execute = &detail::default_healer::operation;
};

#endif  // LORAINE_STAT_HEALTH_H
