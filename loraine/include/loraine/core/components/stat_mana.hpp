

#ifndef LORAINE_STAT_MANA_HPP
#define LORAINE_STAT_MANA_HPP

struct ManaStat {
   /// the mana it costs to play the card (as default)
   long cost_ref;
   /// the actual cost at the moment
   long cost_cache;
   /// the change to the mana cost of the card
   long cost_delta = 0;
};


namespace detail {

struct default_changer {
   static long
   operation(entt::registry& registry, entt::entity card, entt::entity source, long amount)
   {
      auto& mana_c = registry.get<ManaStat>(card);
      auto effective_change = amount;
      if(amount < 0) {
         effective_change = -std::min(mana_c.cost_cache, -amount);
      }
      mana_c.cost_delta += amount;
      update(mana_c);
      return effective_change;
   }

   static inline void update(ManaStat& comp) {
      comp.cost_cache = std::max(0L, comp.cost_delta + comp.cost_ref);
   }
};

}  // namespace detail

struct ManaChanger {
   using FuncType = std::function<
      long(entt::registry& registry, entt::entity unit, entt::entity source, long amount) >;
   FuncType execute = &detail::default_changer::operation;
};


#endif  // LORAINE_STAT_MANA_HPP
