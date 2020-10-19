
#ifndef LORAINE_EFFECT_H
#define LORAINE_EFFECT_H

#include <functional>
#include <utility>

#include "utils.h"
#include "rulesets.h"
#include "types.h"

// forward-declarations
class Game;
class Card;
class Target;

namespace events {
class AnyEvent;
}

enum class EffectType { AOE = 0, TARGETED, NEXUS, OTHER };

class EffectContainer {
  public:
   using EffectFunc = std::function< void(Game&, const events::AnyEvent&, EffectContainer&) >;
   using ConditionFunc = std::function< bool(
      const Game&, const events::AnyEvent&, const EffectContainer&) >;
   using TargetFunc = std::function< void(const Game&, Player, EffectContainer&) >;
   using TargetVerificationFunc = std::function< bool(
      const Game&,
      Player,
      const std::optional< std::vector< Target > >& targets,
      const EffectContainer&) >;

   [[nodiscard]] bool is_null() const { return m_is_null; }
   [[nodiscard]] bool is_consumed() const { return m_consumed; }
   [[nodiscard]] auto get_effect_func() const { return m_effect_func; }
   [[nodiscard]] auto get_cast_con_func() const { return m_cast_con_func; }
   [[nodiscard]] auto get_target_func() const { return m_target_func; }
   [[nodiscard]] auto get_target_verify_func() const { return m_target_verify_func; }
   [[nodiscard]] auto get_targets() const { return m_targets; }
   [[nodiscard]] auto get_value_buffers() const { return m_value_buffers; }
   [[nodiscard]] auto get_associated_card() const { return m_assoc_card; }
   [[nodiscard]] auto get_owner() const { return m_owner; }
   [[nodiscard]] auto get_location() const { return m_location; }
   [[nodiscard]] auto get_effect_type() const { return m_effect_type; }

   inline void consume() { m_consumed = true; }

   inline void set_assoc_card(sptr< Card > card_ptr) { m_assoc_card = std::move(card_ptr); }

   inline void operator()(Game& game, const events::AnyEvent& event)
   {
      m_effect_func(game, event, *this);
   }
   inline void set_targets(std::vector<Target> targets) {m_targets = targets;}

   void choose_targets(const Game& game, Player player) { m_target_func(game, player, *this); }

   [[nodiscard]] inline bool verify_targets(
      const Game& game,
      Player player,
      const std::optional< std::vector< Target > >& opt_targets = {})
   {
      if(opt_targets.has_value()) {
         return m_target_verify_func(game, player, opt_targets.value(), *this);
      }
      return m_target_verify_func(game, player, m_targets, *this);
   }

   [[nodiscard]] inline bool check_cast_condition(
      const Game& game, const events::AnyEvent& event) const
   {
      return m_cast_con_func(game, event, *this);
   }
   bool operator==(const EffectContainer& effect) const
   {
      return m_is_null == effect.is_null() && m_value_buffers == effect.get_value_buffers()
             && get_address(m_effect_func) == get_address(effect.get_effect_func())
             && get_address(m_cast_con_func) == get_address(effect.get_cast_con_func())
             && m_assoc_card == effect.get_associated_card();
   }
   inline bool operator!=(const EffectContainer& effect) const { return ! (*this == effect); }

   ~EffectContainer() = default;

   EffectContainer(
      Player owner,
      EffectFunc effect_func,
      ConditionFunc cast_condition_func,
      Location location,
      EffectType effect_type,
      sptr< Card > card_ptr,
      size_t nr_buffers = 0,
      TargetFunc target_func =
         [](const Game& /*unused*/, Player /*unused*/, EffectContainer& /*unused*/) {},
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
         m_owner(owner),
         m_value_buffers(nr_buffers),
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
         m_consumed(effect.is_consumed()),
         m_owner(effect.get_owner()),
         m_value_buffers(effect.get_value_buffers()),
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
         m_consumed(effect.m_consumed),
         m_owner(effect.m_owner),
         m_value_buffers(std::move(effect.m_value_buffers)),
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
         m_consumed = rhs.m_consumed;
         m_owner = rhs.m_owner;
         m_value_buffers = std::move(rhs.m_value_buffers);
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
   bool m_is_null = false;
   bool m_consumed = false;
   Player m_owner;
   std::optional< std::vector< Target > > m_targets{};
   std::vector< long > m_value_buffers;
   sptr< Card > m_assoc_card;
};

#endif  // LORAINE_EFFECT_H
