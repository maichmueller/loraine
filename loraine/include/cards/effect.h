
#ifndef LORAINE_EFFECT_H
#define LORAINE_EFFECT_H

#include <any>
#include <functional>
#include <utility>

#include "engine/rulesets.h"
#include "events/lor_events/all_events.h"
#include "targeter.h"
#include "utils/types.h"
#include "utils/utils.h"

// forward-declarations
class Game;
class Card;
class Target;
class Agent;

class EffectBase {
  public:
   virtual ~EffectBase() = default;
};

template < typename... Args >
class Effect {
  public:
   enum class Type { AOE = 0, AURA, SIMPLE, TARGETING };
   using SignatureTuple = std::tuple< Args... >;
   using EffectFunc = std::function< void(State&, Effect&, SignatureTuple) >;
   using ConditionFunc = std::function< bool(const State&, const Effect&) >;

   [[nodiscard]] bool is_consumed() const { return m_consumed; }
   inline void consume() { m_consumed = true; }

   [[nodiscard]] auto effect_func() const { return m_effect_func; }
   [[nodiscard]] auto condition_func() const { return m_con_func; }
   [[nodiscard]] auto associated_card() const { return m_assoc_card; }
   [[nodiscard]] auto effect_type() const { return m_effect_type; }
   [[nodiscard]] bool has_targets() const { return not m_targets.empty(); }
   [[nodiscard]] auto targets() const { return m_targets; }
   [[nodiscard]] auto& targets() { return m_targets; }
   [[nodiscard]] auto get_targeter() const { return m_targeter; }
   [[nodiscard]] auto& get_targeter() { return m_targeter; }

   [[nodiscard]] auto& uuid() { return m_uuid; }
   inline void set_targets(std::vector< Target > targets) { m_targets = std::move(targets); }

   inline void reset_targets() { m_targets.clear(); }

   bool operator()(State& state, SignatureTuple arg_tuple);

   [[nodiscard]] inline bool check_condition(const State& state) const
   {
      return m_con_func(state, *this);
   }
   [[nodiscard]] virtual bool target(const State& state, Agent& agent, Player player);

   bool operator==(const Effect& effect) const;
   bool operator!=(const Effect& effect) const;

   Effect(
      EffectFunc effect_func,
      ConditionFunc cast_condition_func,
      sptr< Card > card_ptr,
      sptr< TargeterBase > targeter = std::make_shared< NoneTargeter >(),
      Type effect_type = Type::SIMPLE)
       : m_effect_func(std::move(effect_func)),
         m_con_func(std::move(cast_condition_func)),
         m_effect_type(effect_type),
         m_assoc_card(std::move(card_ptr)),
         m_targeter(std::move(targeter)),
         m_uuid(new_uuid())
   {
   }
   Effect(const Effect& effect) = default;
   Effect(Effect&& effect) noexcept = default;
   Effect& operator=(Effect&& rhs) = default;
   Effect& operator=(const Effect& rhs) = default;

  private:
   const EffectFunc m_effect_func;
   const ConditionFunc m_con_func;
   Type m_effect_type;
   bool m_consumed = false;
   sptr< Card > m_assoc_card;
   sptr< TargeterBase > m_targeter;
   std::vector< Target > m_targets;
   UUID m_uuid;
};

template < typename... Args >
bool Effect< Args... >::operator()(State& state, SignatureTuple arg_tuple)
{
   bool executable = check_condition(state);
   if(executable) {
      m_effect_func(state, *this, std::move(arg_tuple));
   }
   return executable;
}
template < typename... Args >
bool Effect< Args... >::operator==(const Effect& effect) const
{
   return m_uuid == effect.uuid();
}
template < typename... Args >
bool Effect< Args... >::operator!=(const Effect& effect) const
{
   return not (*this == effect);
}

class AttackEffect: public Effect< events::AttackEvent::SignatureTuple > {
};
class BeholdEffect: public Effect< events::BeholdEvent::SignatureTuple > {
};
class BlockEffect: public Effect< events::BlockEvent::SignatureTuple > {
};
class CaptureEffect: public Effect< events::CaptureEvent::SignatureTuple > {
};
class CastEffect: public Effect< events::CastEvent::SignatureTuple > {
};
class DaybreakEffect: public Effect< events::DaybreakEvent::SignatureTuple > {
};
class DieEffect: public Effect< events::DieEvent::SignatureTuple > {
};
class DiscardEffect: public Effect< events::DiscardEvent::SignatureTuple > {
};
class DrawEffect: public Effect< events::DrawCardEvent::SignatureTuple > {
};
class EnlightenmentEffect: public Effect< events::EnlightenmentEvent::SignatureTuple > {
};
class GainManagemEffect: public Effect< events::GainManagemEvent::SignatureTuple > {
};
class HealUnitEffect: public Effect< events::HealUnitEvent::SignatureTuple > {
};
class LevelUpEffect: public Effect< events::LevelUpEvent::SignatureTuple > {
};
class NexusStrikeEffect: public Effect< events::NexusStrikeEvent::SignatureTuple > {
};
class NightfallEffect: public Effect< events::NightfallEvent::SignatureTuple > {
};
class PlayEffect: public Effect< events::PlayEvent::SignatureTuple > {
};
class RecallEffect: public Effect< events::RecallEvent::SignatureTuple > {
};
class RoundEndEffect: public Effect< events::RoundEndEvent::SignatureTuple > {
};
class RoundStartEffect: public Effect< events::RoundStartEvent::SignatureTuple > {
};
class ScoutEffect: public Effect< events::ScoutEvent::SignatureTuple > {
};
class StrikeEffect: public Effect< events::StrikeEvent::SignatureTuple > {
};
class SummonEffect: public Effect< events::SummonEvent::SignatureTuple > {
};
class StunEffect: public Effect< events::StunEvent::SignatureTuple > {
};
class SupportEffect: public Effect< events::SupportEvent::SignatureTuple > {
};
class TargetEffect: public Effect< events::TargetEvent::SignatureTuple > {
};
class UnitDamageEffect: public Effect< events::UnitDamageEvent::SignatureTuple > {
};

#endif  // LORAINE_EFFECT_H
