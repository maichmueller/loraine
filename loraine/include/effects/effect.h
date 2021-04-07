
#ifndef LORAINE_EFFECT_H
#define LORAINE_EFFECT_H

#include "core/gamedefs.h"
#include "core/targeting.h"
#include "events/event_listener.h"
#include "events/eventbase.h"
#include "events/lor_events/event_types.h"
#include "utils/types.h"
#include "utils/utils.h"

// forward-declarations
class GameMode;
class Card;
class Controller;

/**
 * The base class for effects in the game.
 */

class EffectBase: public Cloneable< EffectBase >, public Targeting {
  public:
   enum class Label { AOE = 0, AURA, SIMPLE, TARGETING };
   enum class RegistrationTime {
      CREATION = 0,
      DRAW,
      DEATH,
      SUMMON,
   };

   explicit EffectBase(
      sptr< Card > card_ptr,
      RegistrationTime reg_time,
      const sptr< TargeterBase >& targeter = std::make_shared< NoneTargeter >(),
      Label type = Label::SIMPLE)
       : Targeting(targeter),
         m_effect_label(type),
         m_reg_time(reg_time),
         m_assoc_card(std::move(card_ptr)),
         m_uuid(utils::new_uuid())
   {
   }
   EffectBase(const EffectBase& effect);
   EffectBase(EffectBase&& effect) noexcept = default;
   EffectBase& operator=(EffectBase&& rhs) = default;
   EffectBase& operator=(const EffectBase& rhs) = default;
   ~EffectBase() override = default;

   [[nodiscard]] bool is_consumed() const { return m_consumed; }
   inline void consume() { m_consumed = true; }

   void associated_card(sptr< Card > card) { m_assoc_card = std::move(card); }
   [[nodiscard]] auto associated_card() const { return m_assoc_card; }
   [[nodiscard]] auto label() const { return m_effect_label; }
   [[nodiscard]] auto registration_time() const { return m_reg_time; }

   [[nodiscard]] auto& uuid() const { return m_uuid; }

   bool operator==(const EffectBase& effect) const;
   bool operator!=(const EffectBase& effect) const;

  private:
   Label m_effect_label;
   RegistrationTime m_reg_time;
   bool m_consumed = false;
   sptr< Card > m_assoc_card;
   UUID m_uuid;
};

namespace effects {

/**
 * Any concrete effect implementation has to
 * inherit from this base to be eligible as subscriber to the event in the game.
 * @tparam Events
 */
template < typename... Events >
class Effect:
    public Cloneable< Effect< Events... >, inherit_constructors< EffectBase > >,
    public EventListener< Effect< Events... > >,
    public EventSubscriberInterface< Events... > {
};

using AttackEffect = Effect< events::AttackEvent >;
using BeholdEffect = Effect< events::BeholdEvent >;
using BlockEffect = Effect< events::BlockEvent >;
using CaptureEffect = Effect< events::CaptureEvent >;
using CastEffect = Effect< events::CastEvent >;
using DaybreakEffect = Effect< events::DaybreakEvent >;
using DiscardEffect = Effect< events::DiscardEvent >;
using DrawCardEffect = Effect< events::DrawCardEvent >;
using EnlightenmentEffect = Effect< events::EnlightenmentEvent >;
using GainManagemEffect = Effect< events::GainManagemEvent >;
using HealUnitEffect = Effect< events::HealUnitEvent >;
using LevelUpEffect = Effect< events::LevelUpEvent >;
using NexusDamageEffect = Effect< events::NexusDamageEvent >;
using NexusStrikeEffect = Effect< events::NexusStrikeEvent >;
using NightfallEffect = Effect< events::NightfallEvent >;
using PlayEffect = Effect< events::PlayEvent >;
using RecallEffect = Effect< events::RecallEvent >;
using RoundEndEffect = Effect< events::RoundEndEvent >;
using RoundStartEffect = Effect< events::RoundStartEvent >;
using ScoutEffect = Effect< events::ScoutEvent >;
using SlayEffect = Effect< events::SlayEvent >;
using StrikeEffect = Effect< events::StrikeEvent >;
using StunEffect = Effect< events::StunEvent >;
using SummonEffect = Effect< events::SummonEvent >;
using SupportEffect = Effect< events::SupportEvent >;
using TargetEffect = Effect< events::TargetEvent >;
using UnitDamageEffect = Effect< events::UnitDamageEvent >;

}  // namespace effects

namespace helpers {

template < events::EventLabel label >
struct eventlabel_to_effect;

template <>
struct eventlabel_to_effect< events::EventLabel::ATTACK > {
   using type = effects::AttackEffect;
   using label_type = events::EventLabelType< events::EventLabel::ATTACK >;
};
template <>
struct eventlabel_to_effect< events::EventLabel::BEHOLD > {
   using type = effects::BeholdEffect;
   using label_type = events::EventLabelType< events::EventLabel::BEHOLD >;
};
template <>
struct eventlabel_to_effect< events::EventLabel::BLOCK > {
   using type = effects::BlockEffect;
   using label_type = events::EventLabelType< events::EventLabel::BLOCK >;
};
template <>
struct eventlabel_to_effect< events::EventLabel::CAPTURE > {
   using type = effects::CaptureEffect;
   using label_type = events::EventLabelType< events::EventLabel::CAPTURE >;
};
template <>
struct eventlabel_to_effect< events::EventLabel::CAST > {
   using type = effects::CastEffect;
   using label_type = events::EventLabelType< events::EventLabel::CAST >;
};
template <>
struct eventlabel_to_effect< events::EventLabel::DAYBREAK > {
   using type = effects::DaybreakEffect;
   using label_type = events::EventLabelType< events::EventLabel::DAYBREAK >;
};

template <>
struct eventlabel_to_effect< events::EventLabel::DISCARD > {
   using type = effects::DiscardEffect;
   using label_type = events::EventLabelType< events::EventLabel::DISCARD >;
};
template <>
struct eventlabel_to_effect< events::EventLabel::DRAW_CARD > {
   using type = effects::DrawCardEffect;
   using label_type = events::EventLabelType< events::EventLabel::DRAW_CARD >;
};
template <>
struct eventlabel_to_effect< events::EventLabel::ENLIGHTENMENT > {
   using type = effects::EnlightenmentEffect;
   using label_type = events::EventLabelType< events::EventLabel::ENLIGHTENMENT >;
};
template <>
struct eventlabel_to_effect< events::EventLabel::GAIN_MANAGEM > {
   using type = effects::GainManagemEffect;
   using label_type = events::EventLabelType< events::EventLabel::GAIN_MANAGEM >;
};
template <>
struct eventlabel_to_effect< events::EventLabel::HEAL_UNIT > {
   using type = effects::HealUnitEffect;
   using label_type = events::EventLabelType< events::EventLabel::HEAL_UNIT >;
};
template <>
struct eventlabel_to_effect< events::EventLabel::LEVEL_UP > {
   using type = effects::LevelUpEffect;
   using label_type = events::EventLabelType< events::EventLabel::LEVEL_UP >;
};
template <>
struct eventlabel_to_effect< events::EventLabel::NEXUS_DAMAGE > {
   using type = effects::NexusDamageEffect;
   using label_type = events::EventLabelType< events::EventLabel::NEXUS_DAMAGE >;
};
template <>
struct eventlabel_to_effect< events::EventLabel::NEXUS_STRIKE > {
   using type = effects::NexusStrikeEffect;
   using label_type = events::EventLabelType< events::EventLabel::NEXUS_STRIKE >;
};
template <>
struct eventlabel_to_effect< events::EventLabel::NIGHTFALL > {
   using type = effects::NightfallEffect;
   using label_type = events::EventLabelType< events::EventLabel::NIGHTFALL >;
};
template <>
struct eventlabel_to_effect< events::EventLabel::PLAY > {
   using type = effects::PlayEffect;
   using label_type = events::EventLabelType< events::EventLabel::PLAY >;
};
template <>
struct eventlabel_to_effect< events::EventLabel::RECALL > {
   using type = effects::RecallEffect;
   using label_type = events::EventLabelType< events::EventLabel::RECALL >;
};
template <>
struct eventlabel_to_effect< events::EventLabel::ROUND_END > {
   using type = effects::RoundEndEffect;
   using label_type = events::EventLabelType< events::EventLabel::ROUND_END >;
};
template <>
struct eventlabel_to_effect< events::EventLabel::ROUND_START > {
   using type = effects::RoundStartEffect;
   using label_type = events::EventLabelType< events::EventLabel::ROUND_START >;
};
template <>
struct eventlabel_to_effect< events::EventLabel::SCOUT > {
   using type = effects::ScoutEffect;
   using label_type = events::EventLabelType< events::EventLabel::SCOUT >;
};
template <>
struct eventlabel_to_effect< events::EventLabel::SLAY > {
   using type = effects::SlayEffect;
   using label_type = events::EventLabelType< events::EventLabel::SLAY >;
};
template <>
struct eventlabel_to_effect< events::EventLabel::STRIKE > {
   using type = effects::StrikeEffect;
   using label_type = events::EventLabelType< events::EventLabel::STRIKE >;
};
template <>
struct eventlabel_to_effect< events::EventLabel::SUMMON > {
   using type = effects::SummonEffect;
   using label_type = events::EventLabelType< events::EventLabel::SUMMON >;
};
template <>
struct eventlabel_to_effect< events::EventLabel::STUN > {
   using type = effects::StunEffect;
   using label_type = events::EventLabelType< events::EventLabel::STUN >;
};
template <>
struct eventlabel_to_effect< events::EventLabel::SUPPORT > {
   using type = effects::SupportEffect;
   using label_type = events::EventLabelType< events::EventLabel::SUPPORT >;
};
template <>
struct eventlabel_to_effect< events::EventLabel::TARGET > {
   using type = effects::TargetEffect;
   using label_type = events::EventLabelType< events::EventLabel::TARGET >;
};
template <>
struct eventlabel_to_effect< events::EventLabel::UNIT_DAMAGE > {
   using type = effects::UnitDamageEffect;
   using label_type = events::EventLabelType< events::EventLabel::UNIT_DAMAGE >;
};

template < events::EventLabel elabel >
using eventlabel_to_effect_t = typename eventlabel_to_effect< elabel >::type;

}  // namespace helpers
#endif  // LORAINE_EFFECT_H
