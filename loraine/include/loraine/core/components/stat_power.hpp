
#ifndef LORAINE_STAT_POWER_HPP
#define LORAINE_STAT_POWER_HPP

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
      return effective_buff;
   }

};

}  // namespace detail

struct Buffer {
   using FuncType = std::function<
      long(entt::registry& registry, entt::entity unit, entt::entity source, long amount) >;

   Buffer() : executor() { reset(); }
   Buffer(FuncType f) : executor(f) {}

   inline void reset() { executor = &detail::default_buffer::operation; }

   FuncType executor;
};


#endif  // LORAINE_STAT_POWER_HPP
