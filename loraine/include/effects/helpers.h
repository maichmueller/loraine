
#ifndef LORAINE_HELPERS_H
#define LORAINE_HELPERS_H

#include "effect.h"

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

#endif  // LORAINE_HELPERS_H
