
#ifndef LORAINE_EFFECT_H
#define LORAINE_EFFECT_H

#include <event/event.h>

#include <functional>
#include <utility>

// forward-declarations
class Game;
class Card;

struct Target {
   bool is_nexus;
   Player player;
   Location loc;
   size_t index;
   sptr< Card > card;
};

enum class EffectType { AOE = 0, TARGETED, NEXUS, OTHER };

class EffectContainer {
  public:
   using EffectFunc = std::function< void(
      Game&, const events::VariantEvent&, EffectContainer&) >;
   using ConditionFunc = std::function< bool(
      const Game&, const events::VariantEvent&, const EffectContainer&) >;
   using TargetFunc = std::function< void(
      const Game&, Player, EffectContainer&) >;
   using TargetVerificationFunc = std::function< bool(
      const Game&,
      Player,
      const std::optional< std::vector< Target > >& targets,
      const EffectContainer&) >;

   [[nodiscard]] bool is_null() const { return m_is_null; }
   [[nodiscard]] auto get_effect_func() const { return m_effect_func; }
   [[nodiscard]] auto get_cast_con_func() const { return m_cast_con_func; }
   [[nodiscard]] auto get_target_func() const { return m_target_func; }
   [[nodiscard]] auto get_target_verify_func() const
   {
      return m_target_verify_func;
   }
   [[nodiscard]] auto get_targets() const { return m_targets; }
   [[nodiscard]] auto get_counter_round() const { return m_counter_rnd; }
   [[nodiscard]] auto get_counter_total() const { return m_counter_total; }
   [[nodiscard]] auto get_associated_card() const { return m_assoc_card; }
   [[nodiscard]] auto get_owner() const { return m_owner; }
   [[nodiscard]] auto get_location() const { return m_location; }
   [[nodiscard]] auto get_effect_type() const { return m_effect_type; }

   void reset_counter_round() { m_counter_rnd = 0; }
   void set_assoc_card(sptr< Card > card_ptr)
   {
      m_assoc_card = std::move(card_ptr);
   }

   void operator()(Game& game, const events::VariantEvent& event)
   {
      m_effect_func(game, event, *this);
      m_counter_rnd += 1;
      m_counter_total += 1;
   }

   void choose_targets(const Game& game, Player player)
   {
      m_target_func(game, player, *this);
   }
   [[nodiscard]] bool verify_targets(
      const Game& game,
      Player player,
      const std::optional< std::vector< Target > >& opt_targets = {}) const
   {
      if(opt_targets.has_value()) {
         return m_target_verify_func(game, player, opt_targets, *this);
      }
      return m_target_verify_func(game, player, m_targets, *this);
   }

   [[nodiscard]] bool check_cast_condition(
      const Game& game, const events::VariantEvent& event) const
   {
      return m_cast_con_func(game, event, *this);
   }
   bool operator==(const EffectContainer& effect) const
   {
      return m_is_null == effect.is_null()
             && m_counter_rnd == effect.get_counter_round()
             && m_counter_total == effect.get_counter_total()
             && _get_address(m_effect_func)
                   == _get_address(effect.get_effect_func())
             && _get_address(m_cast_con_func)
                   == _get_address(effect.get_cast_con_func())
             && m_assoc_card == effect.get_associated_card();
   }
   bool operator!=(const EffectContainer& effect) const
   {
      return ! (*this == effect);
   }

   EffectContainer(
      Player owner,
      EffectFunc effect_func,
      ConditionFunc cast_condition_func,
      Location location,
      EffectType effect_type,
      sptr< Card > card_ptr,
      TargetFunc target_func = [](const Game& /*unused*/,
                                  Player /*unused*/,
                                  EffectContainer& /*unused*/) {},
      TargetVerificationFunc target_verify_func =
         [](const Game& /*unused*/,
            Player /*unused*/,
            const std::optional< std::vector< Target > >& /*unused*/,
            const EffectContainer& /*unused*/) { return true; })
       : m_effect_func(std::move(effect_func)),
         m_cast_con_func(std::move(cast_condition_func)),
         m_target_func(std::move(target_func)),
         m_target_verify_func(std::move(target_verify_func)),
         m_location(location),
         m_effect_type(effect_type),
         m_is_null(false),
         m_owner(owner),
         m_assoc_card(std::move(card_ptr))
   {
   }
   EffectContainer(const EffectContainer& effect)
       : m_effect_func(effect.get_effect_func()),
         m_cast_con_func(effect.get_cast_con_func()),
         m_target_func(effect.get_target_func()),
         m_target_verify_func(effect.get_target_verify_func()),
         m_location(effect.get_location()),
         m_effect_type(effect.get_effect_type()),
         m_is_null(effect.is_null()),
         m_owner(effect.get_owner()),
         m_counter_rnd(effect.get_counter_round()),
         m_counter_total(effect.get_counter_total()),
         m_assoc_card(effect.get_associated_card())
   {
   }
   EffectContainer(EffectContainer&& effect) noexcept
       : m_effect_func(std::move(effect.m_effect_func)),
         m_cast_con_func(std::move(effect.m_cast_con_func)),
         m_target_func(std::move(effect.m_target_func)),
         m_location(effect.m_location),
         m_effect_type(effect.m_effect_type),
         m_is_null(effect.m_is_null),
         m_owner(effect.m_owner),
         m_counter_rnd(effect.m_counter_rnd),
         m_counter_total(effect.m_counter_total),
         m_assoc_card(std::move(effect.m_assoc_card))
   {
   }

   EffectContainer& operator=(EffectContainer&& rhs) noexcept
   {
      if(*this != rhs) {
         m_effect_func = std::move(rhs.m_effect_func);
         m_cast_con_func = std::move(rhs.m_cast_con_func);
         m_target_func = std::move(rhs.m_target_func);
         m_location = rhs.m_location;
         m_effect_type = rhs.m_effect_type;
         m_is_null = rhs.m_is_null;
         m_owner = rhs.m_owner;
         m_counter_rnd = rhs.m_counter_rnd;
         m_counter_total = rhs.m_counter_total;
         m_assoc_card = rhs.m_assoc_card;
      }
      return *this;
   }
   EffectContainer& operator=(const EffectContainer& rhs) = delete;

  private:
   EffectFunc m_effect_func;
   ConditionFunc m_cast_con_func;
   TargetFunc m_target_func;
   TargetVerificationFunc m_target_verify_func;
   Location m_location;
   EffectType m_effect_type;
   bool m_is_null;
   Player m_owner;
   std::optional< std::vector< Target > > m_targets{};
   size_t m_counter_rnd{0};
   size_t m_counter_total{0};
   sptr< Card > m_assoc_card;

   /*
    * Get the address of the function pointer.
    *
    * Needed for effect function comparison of equality.
    *
    * this may need some further checkups for portability, as conversion to
    * size_t of a function pointer is implementation dependent.
    */
   template < typename ReturnType, typename... Params >
   size_t _get_address(std::function< ReturnType(Params...) > f) const
   {
      using FuncType = ReturnType(Params...);
      auto** fn_ptr = f.template target< FuncType* >();
      return size_t(*fn_ptr);
   }
};

class AoeContainer: public EffectContainer {
};

#endif  // LORAINE_EFFECT_H
