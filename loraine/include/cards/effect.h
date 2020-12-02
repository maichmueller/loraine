
#ifndef LORAINE_EFFECT_H
#define LORAINE_EFFECT_H

#include <any>
#include <functional>
#include <utility>

#include "rulesets.h"
#include "targeter.h"
#include "types.h"
#include "utils.h"
#include "uuid_gen.h"

// forward-declarations
class Game;
class Card;
class Target;

namespace events {
class AnyEvent;
}

class Effect {
  public:
   enum class Type { AOE = 0, AURA, SIMPLE, TARGETED };

   using EffectFunc = std::function< void(Game&, const events::AnyEvent&, Effect&) >;
   using ConditionFunc = std::function< bool(const Game&, const Effect&) >;

   [[nodiscard]] bool is_null() const { return m_is_null; }
   [[nodiscard]] bool is_consumed() const { return m_consumed; }
   [[nodiscard]] auto get_effect_func() const { return m_effect_func; }
   [[nodiscard]] auto get_condition_func() const { return m_con_func; }
   [[nodiscard]] auto get_associated_card() const { return m_assoc_card; }
   [[nodiscard]] auto get_location() const { return m_location; }
   [[nodiscard]] auto get_effect_type() const { return m_effect_type; }
   [[nodiscard]] Player get_owner() const;

   inline void consume() { m_consumed = true; }

   bool operator()(Game& game, const events::AnyEvent& event);

   [[nodiscard]] inline bool check_condition(const Game& game) const
   {
      return m_con_func(game, *this);
   }
   bool operator==(const Effect& effect) const;
   bool operator!=(const Effect& effect) const;

   virtual ~Effect() = default;

   Effect(
      EffectFunc effect_func,
      ConditionFunc cast_condition_func,
      Location location,
      sptr< Card > card_ptr,
      Type effect_type = Type::SIMPLE)
       : m_effect_func(std::move(effect_func)),
         m_con_func(std::move(cast_condition_func)),
         m_location(location),
         m_effect_type(effect_type),
         m_assoc_card(std::move(card_ptr))
   {
   }
   Effect(const Effect& effect) = default;
   Effect(Effect&& effect) noexcept = default;
   Effect& operator=(Effect&& rhs) = default;
   Effect& operator=(const Effect& rhs) = default;

  private:
   EffectFunc m_effect_func;
   ConditionFunc m_con_func;
   Location m_location;
   Type m_effect_type;
   bool m_is_null = false;
   bool m_consumed = false;
   sptr< Card > m_assoc_card;

   virtual void _call(Game& game, const events::AnyEvent& event);
};

class TargetEffect: public Effect {
  public:
   template < typename... Params >
   TargetEffect(
      Params... params, sptr< BaseTargeter > targeter = std::make_shared< NoneTargeter >())
       : Effect(std::forward< Params... >(params...)), m_targeter(std::move(targeter))
   {
   }

   [[nodiscard]] bool has_targets() const { return not m_targets.empty(); }
   [[nodiscard]] auto get_targets() const { return m_targets; }
   inline void set_targets(std::vector< Target > targets) { m_targets = std::move(targets); }
   inline void reset_targets() { m_targets.clear(); }

   [[nodiscard]] inline auto find_available_targets(const State& state, Player player)
   {
      return (*m_targeter)(state, player);
   }

   [[nodiscard]] auto get_targeter() const { return m_targeter; }

  private:
   sptr< BaseTargeter > m_targeter;
   std::vector< Target > m_targets;

   void _call(Game& game, const events::AnyEvent& event) override;
};

#endif  // LORAINE_EFFECT_H
