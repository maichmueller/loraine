
#ifndef LORAINE_EFFECT_H
#define LORAINE_EFFECT_H

#include <any>
#include <functional>
#include <utility>

#include "engine/gamedefs.h"
#include "events/lor_events/all_events.h"
#include "targeter.h"
#include "utils/types.h"
#include "utils/utils.h"

// forward-declarations
class GameMode;
class Card;
class Target;
class Controller;

class EffectBase {
  public:
   virtual ~EffectBase() = default;
};

template < typename... Args >
class Effect : public EffectBase {
  public:
   using SignatureTuple = std::tuple< Args... >;
   enum class Type { AOE = 0, AURA, SIMPLE, TARGETING };

   /**
    * The call to trigger the effect's changes on the status. Calls internally the private function
    * `_event_call`.
    * @param state State,
    *   the state of the associated game
    * @param args Parameter-Pack,
    *   the specific arguments of the subscribed to event
    */
   void event_call(State& state, Args... args);
   [[nodiscard]] inline bool check_condition(const State& state) const { return _condition(state); }
   [[nodiscard]] virtual bool target(const State& state, Controller& agent, Team team);
   [[nodiscard]] bool is_consumed() const { return m_consumed; }
   inline void consume() { m_consumed = true; }

   [[nodiscard]] auto associated_card() const { return m_assoc_card; }
   [[nodiscard]] auto effect_type() const { return m_effect_type; }

   inline void targets(std::vector< Target > targets) { m_targets = std::move(targets); }
   [[nodiscard]] bool has_targets() const { return not m_targets.empty(); }
   [[nodiscard]] auto& targets() { return m_targets; }
   [[nodiscard]] auto targets() const { return m_targets; }

   inline void reset_targets() { m_targets.clear(); }
   [[nodiscard]] auto targeter() const { return m_targeter; }
   [[nodiscard]] auto& targeter() { return m_targeter; }

   [[nodiscard]] auto& uuid() { return m_uuid; }

   bool operator==(const Effect& effect) const;
   bool operator!=(const Effect& effect) const;

   Effect(
      sptr< Card > card_ptr,
      sptr< TargeterBase > targeter = std::make_shared< NoneTargeter >(),
      Type effect_type = Type::SIMPLE)
       : m_effect_type(effect_type),
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
   Type m_effect_type;
   bool m_consumed = false;
   sptr< Card > m_assoc_card;
   sptr< TargeterBase > m_targeter;
   std::vector< Target > m_targets;
   UUID m_uuid;

   /**
    * The actual effect function to trigger upon the depending event happening.
    * @param state State,
    *   the current state of the associated game
    * @param args: Parameter-Pack,
    *   the specific arguments of the subscribed to event
    */
   virtual void _event_call(State& state, Args... args) = 0;
   /**
    * Provides the boolean status of the condition for this effect to be executed.
    * @param state State,
    *   the current state of the associated game
    * @return boolean,
    *   whether this effect can be casted
    */
   [[nodiscard]] virtual bool _condition(const State& state) const { return true; };
};

template < typename... Args >
void Effect< Args... >::event_call(State& state, Args... args)
{
   if(check_condition(state)) {
      _event_call(state, args...);
   }
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

using AttackEffect = events::AttackEvent::Subscriber;
using BeholdEffect = events::BeholdEvent::Subscriber;
using BlockEffect = events::BlockEvent::Subscriber;
using CaptureEffect = events::CaptureEvent::Subscriber;
using CastEffect = events::CastEvent::Subscriber;
using DaybreakEffect = events::DaybreakEvent::Subscriber;
using DieEffect = events::DieEvent::Subscriber;
using DiscardEffect = events::DiscardEvent::Subscriber;
using DrawEffect = events::DrawCardEvent::Subscriber;
using EnlightenmentEffect = events::EnlightenmentEvent::Subscriber;
using GainManagemEffect = events::GainManagemEvent::Subscriber;
using HealUnitEffect = events::HealUnitEvent::Subscriber;
using LevelUpEffect = events::LevelUpEvent::Subscriber;
using NexusStrikeEffect = events::NexusStrikeEvent::Subscriber;
using NightfallEffect = events::NightfallEvent::Subscriber;
using PlayEffect = events::PlayEvent::Subscriber;
using RecallEffect = events::RecallEvent::Subscriber;
using RoundEndEffect = events::RoundEndEvent::Subscriber;
using RoundStartEffect = events::RoundStartEvent::Subscriber;
using ScoutEffect = events::ScoutEvent::Subscriber;
using StrikeEffect = events::StrikeEvent::Subscriber;
using SummonEffect = events::SummonEvent::Subscriber;
using StunEffect = events::StunEvent::Subscriber;
using SupportEffect = events::SupportEvent::Subscriber;
using TargetEffect = events::TargetEvent::Subscriber;
using UnitDamageEffect = events::UnitDamageEvent::Subscriber;

using LOREffect = std::variant<
   AttackEffect,
   BeholdEffect,
   BlockEffect,
   CaptureEffect,
   CastEffect,
   DaybreakEffect,
   DieEffect,
   DiscardEffect,
   DrawEffect,
   EnlightenmentEffect,
   GainManagemEffect,
   HealUnitEffect,
   AttackEffect,
   LevelUpEffect,
   NexusStrikeEffect,
   NightfallEffect,
   PlayEffect,
   RecallEffect,
   RoundEndEffect,
   RoundStartEffect,
   ScoutEffect,
   StrikeEffect,
   SummonEffect,
   StunEffect,
   SupportEffect,
   TargetEffect,
   UnitDamageEffect >;

#endif  // LORAINE_EFFECT_H
