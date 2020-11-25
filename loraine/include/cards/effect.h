
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

class EffectContainer {
  public:
   enum class Type { AOE = 0, TARGETED, NEXUS, OTHER };

   using EffectFunc = std::function< void(Game&, const events::AnyEvent&, EffectContainer&) >;
   using ConditionFunc = std::function< bool(const Game&, const EffectContainer&) >;

   [[nodiscard]] bool has_targets() const { return ! m_targets.empty(); }
   [[nodiscard]] bool is_null() const { return m_is_null; }
   [[nodiscard]] bool is_consumed() const { return m_consumed; }
   [[nodiscard]] auto get_effect_func() const { return m_effect_func; }
   [[nodiscard]] auto get_cast_condition_func() const { return m_cast_con_func; }
   [[nodiscard]] auto get_targets() const { return m_targets; }
   [[nodiscard]] auto get_value_buffers() const { return m_value_buffers; }
   [[nodiscard]] auto get_associated_card() const { return m_assoc_card; }
   [[nodiscard]] auto get_location() const { return m_location; }
   [[nodiscard]] auto get_effect_type() const { return m_effect_type; }
   [[nodiscard]] auto get_targeter() const { return m_targeter; }
   [[nodiscard]] Player get_owner() const;

   inline void consume() { m_consumed = true; }

   void operator()(Game& game, const events::AnyEvent& event);
   inline void set_targets(std::vector< Target > targets) { m_targets = std::move(targets); }

   void choose_targets(const State& state, Agent& agent, Player player)
   {
      (*m_targeter)(state, agent, player);
   }

   [[nodiscard]] inline bool check_cast_condition(const Game& game) const
   {
      return m_cast_con_func(game, *this);
   }
   bool operator==(const EffectContainer& effect) const;
   bool operator!=(const EffectContainer& effect) const;

   ~EffectContainer() = default;

   EffectContainer(
      EffectFunc effect_func,
      ConditionFunc cast_condition_func,
      Location location,
      Type effect_type,
      sptr< Card > card_ptr,
      size_t nr_buffers = 0,
      sptr< BaseTargeter > targeter = std::make_shared< NoneTargeter >())
       : m_effect_func(std::move(effect_func)),
         m_cast_con_func(std::move(cast_condition_func)),
         m_targeter(std::move(targeter)),
         m_location(location),
         m_effect_type(effect_type),
         m_value_buffers(nr_buffers),
         m_assoc_card(std::move(card_ptr))
   {
   }
   EffectContainer(const EffectContainer& effect)
       : m_effect_func(effect.get_effect_func()),
         m_cast_con_func(effect.get_cast_condition_func()),
         m_targeter(effect.get_targeter()),
         m_location(effect.get_location()),
         m_effect_type(effect.get_effect_type()),
         m_is_null(effect.is_null()),
         m_consumed(effect.is_consumed()),
         m_value_buffers(effect.get_value_buffers()),
         m_assoc_card(effect.get_associated_card())
   {
   }
   EffectContainer(EffectContainer&& effect) noexcept
       : m_effect_func(std::move(effect.m_effect_func)),
         m_cast_con_func(std::move(effect.m_cast_con_func)),
         m_targeter(std::move(effect.m_targeter)),
         m_location(effect.m_location),
         m_effect_type(effect.m_effect_type),
         m_is_null(effect.m_is_null),
         m_consumed(effect.m_consumed),
         m_value_buffers(std::move(effect.m_value_buffers)),
         m_assoc_card(std::move(effect.m_assoc_card))
   {
   }
   EffectContainer& operator=(EffectContainer&& rhs) noexcept
   {
      if(*this != rhs) {
         m_effect_func = std::move(rhs.m_effect_func);
         m_cast_con_func = std::move(rhs.m_cast_con_func);
         m_targeter = std::move(rhs.m_targeter);
         m_location = rhs.m_location;
         m_effect_type = rhs.m_effect_type;
         m_is_null = rhs.m_is_null;
         m_consumed = rhs.m_consumed;
         m_targets = rhs.m_targets;
         m_value_buffers = std::move(rhs.m_value_buffers);
         m_assoc_card = rhs.m_assoc_card;
      }
      return *this;
   }
   EffectContainer& operator=(const EffectContainer& rhs) = default;

  private:
   EffectFunc m_effect_func;
   ConditionFunc m_cast_con_func;
   sptr< BaseTargeter > m_targeter;
   Location m_location;
   Type m_effect_type;
   bool m_is_null = false;
   bool m_consumed = false;
   std::vector< Target > m_targets;
   std::vector< std::any > m_value_buffers;
   sptr< Card > m_assoc_card;
};

#endif  // LORAINE_EFFECT_H
