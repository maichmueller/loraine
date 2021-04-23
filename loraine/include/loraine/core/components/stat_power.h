
#ifndef LORAINE_STAT_POWER_H
#define LORAINE_STAT_POWER_H

struct PowerStat {
   /// the mana it costs to play the card (as default)
   long power_ref;
   /// the actual cost at the moment
   long power_cache;
   /// the change to the mana cost of the card
   long int power_delta = 0;
};

namespace detail {

struct default_buffer {
   static long
   operation(entt::registry& registry, entt::entity unit, entt::entity source, long amount)
   {
      auto& power_c = registry.get< PowerStat >(unit);
      auto effective_buff = amount;
      if(amount < 0) {
         effective_buff = -std::min(power_c.power_cache, -amount);
      }
      power_c.power_delta += amount;
      update(power_c);
      return effective_buff;
   }

   static inline void update(PowerStat& comp)
   {
      comp.power_cache = std::max(0L, comp.power_delta + comp.power_ref);
   }
};

}  // namespace detail

struct Buffer {
   using FuncType = std::function<
      long(entt::registry& registry, entt::entity unit, entt::entity source, long amount) >;
   FuncType execute = &detail::default_buffer::operation;
};

#endif  // LORAINE_STAT_POWER_H
