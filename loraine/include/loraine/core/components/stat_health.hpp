
#ifndef LORAINE_STAT_HEALTH_HPP
#define LORAINE_STAT_HEALTH_HPP

struct HealthStat {
   /// the health of the card on_add creation
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
   static uint
   operation(entt::registry& registry, entt::entity damaged_unit, entt::entity source, uint amount)
   {
      auto& comp = registry.get< HealthStat >(damaged_unit);
      auto effective_damage = std::min(comp.health_cache, amount);
      comp.damage += effective_damage;
      return effective_damage;
   }

};

struct default_healer {
   static uint
   operation(entt::registry& registry, entt::entity healed_unit, entt::entity source, uint amount)
   {
      auto& comp = registry.get< HealthStat >(healed_unit);
      auto effective_healing = std::min(amount, comp.damage);
      comp.damage -= effective_healing;
      return effective_healing;
   }


};

}  // namespace detail

struct Damager {
   using FuncType = std::function<
      long(entt::registry& registry, entt::entity unit, entt::entity source, long amount) >;

   Damager() : executor() { reset(); }
   Damager(FuncType f) : executor(f) {}

   inline void reset() { executor = &detail::default_damager::operation; }

   FuncType executor;
};

struct Healer {
   using FuncType = std::function<
      long(entt::registry& registry, entt::entity unit, entt::entity source, long amount) >;

   Healer() : executor() { reset(); }
   Healer(FuncType f) : executor(f) {}

   inline void reset() { executor = &detail::default_healer::operation; }

   FuncType executor;
};
#endif  // LORAINE_STAT_HEALTH_HPP
