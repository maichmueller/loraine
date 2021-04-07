
#ifndef LORAINE_EVENT_TYPES_H
#define LORAINE_EVENT_TYPES_H

#include <variant>

#include "events/event.h"

// forward-declare
class Card;
class FieldCard;
class Unit;
class Spell;
class GameState;

namespace events {

class AttackEvent: public Event< AttackEvent, EventLabelType< EventLabel::ATTACK >, Team > {
};
/*
 * 1st Arg: const sptr<Card>& = Beholder.
 * 1st Arg: const sptr<Card>& = Beholden.
 */
class BeholdEvent:
    public Event<
       BeholdEvent,
       EventLabelType< EventLabel::BEHOLD >,
       Team,
       const sptr< Card >&,
       const sptr< Card >& > {
};
class BlockEvent: public Event< BlockEvent, EventLabelType< EventLabel::BLOCK >, Team > {
};
/*
 * 1st Arg: const sptr<Card>& = Card which triggered the capture
 * 2nd Arg: const sptr<Unit>& = Captor
 * 3rd Arg: const sptr<Unit>& = Captee
 */
class CaptureEvent:
    public Event<
       CaptureEvent,
       EventLabelType< EventLabel::CAPTURE >,
       Team,
       const sptr< Card >&,
       const sptr< Unit >&,
       const sptr< Unit >& > {
};
/*
 * 1st Arg: const sptr<Card>& = The casted floating
 */
class CastEvent:
    public Event< CastEvent, EventLabelType< EventLabel::CAST >, Team, const sptr< Card >& > {
};
/*
 * 1st ARg: const sptr<Card>&: The daybreak causing spell
 */
class DaybreakEvent:
    public Event<
       DaybreakEvent,
       EventLabelType< EventLabel::DAYBREAK >,
       Team,
       const sptr< Card >& > {
};

/*
 * 1st Arg: const sptr<Card>& = The discard reason
 * 2nd Arg: const sptr<Card>& = The discarded spell
 */
class DiscardEvent:
    public Event<
       DiscardEvent,
       EventLabelType< EventLabel::DISCARD >,
       Team,
       const sptr< Card >&,
       const sptr< Card >& > {
};
/*
 * 1st Arg: const sptr<Card>& = The drawn spell
 */
class DrawCardEvent:
    public Event<
       DrawCardEvent,
       EventLabelType< EventLabel::DRAW_CARD >,
       Team,
       const sptr< Card >& > {
};

class EnlightenmentEvent:
    public Event< EnlightenmentEvent, EventLabelType< EventLabel::ENLIGHTENMENT >, Team > {
};
/*
 * 1st Arg: long = The amount of gems gained
 */
class GainManagemEvent:
    public Event< GainManagemEvent, EventLabelType< EventLabel::GAIN_MANAGEM >, Team, long > {
};
/*
 * 1st Arg: const sptr<Card>& = The healing card
 * 2nd Arg: const sptr<Card>& = The healed spell
 * 3rd Arg: long = The amount healed
 */
class HealUnitEvent:
    public Event<
       HealUnitEvent,
       EventLabelType< EventLabel::HEAL_UNIT >,
       Team,
       const sptr< Card >&,
       const sptr< Card >&,
       long > {
};
/*
 * 1st Arg: const sptr<Card>& = The leveling champion spell
 */
class LevelUpEvent:
    public Event<
       LevelUpEvent,
       EventLabelType< EventLabel::LEVEL_UP >,
       Team,
       const sptr< Card >& > {
};

/*
 * 1st Arg: const sptr<Card>& = Nexus damaging spell
 * 2nd Arg: long = The damage
 */
class NexusDamageEvent:
    public Event<
       NexusDamageEvent,
       EventLabelType< EventLabel::NEXUS_DAMAGE >,
       Team,
       const sptr< Card >&,
       long > {
};

/*
 * 1st Arg: const sptr<Card>& = Nexus striking spell
 * 2nd Arg: long = The damage
 */
class NexusStrikeEvent:
    public Event<
       NexusStrikeEvent,
       EventLabelType< EventLabel::NEXUS_STRIKE >,
       Team,
       const sptr< Card >&,
       long > {
};

/*
 * 1st Arg: const sptr<Card>& = The nightfall causing spell
 */
class NightfallEvent:
    public Event<
       NightfallEvent,
       EventLabelType< EventLabel::NIGHTFALL >,
       Team,
       const sptr< Card >& > {
};
/*
 * 1st Arg: const sptr<Card>& = The played spell
 */
class PlayEvent:
    public Event< PlayEvent, EventLabelType< EventLabel::ATTACK >, Team, const sptr< Card >& > {
};
/*
 * 1st Arg: const sptr<Card>& = The recalling spell
 * 2nd Arg: const sptr<Card>& = The recalled card
 */
class RecallEvent:
    public Event<
       RecallEvent,
       EventLabelType< EventLabel::RECALL >,
       Team,
       const sptr< Card >&,
       const sptr< Card >& > {
};
/*
 * 1st Arg: long = The round number
 */
class RoundEndEvent:
    public Event< RoundEndEvent, EventLabelType< EventLabel::ROUND_END >, Team, long > {
};
/*
 * 1st Arg: long = The round number
 */
class RoundStartEvent:
    public Event< RoundStartEvent, EventLabelType< EventLabel::ROUND_START >, Team, long > {
};

class ScoutEvent: public Event< ScoutEvent, EventLabelType< EventLabel::SCOUT >, Team > {
};
/*
 * 1st Arg: const sptr<Card>& = The killing card
 * 2nd Arg:const sptr<Unit>& = The killed unit
 */
class SlayEvent:
    public Event<
       SlayEvent,
       EventLabelType< EventLabel::SLAY >,
       Team,
       const sptr< Card >&,
       const sptr< Unit >& > {
};
/*
 * 1st ARg: const sptr<Unit>& = The striking unit
 * 2nd Arg: const sptr<Unit>& = The struck unit
 */
class StrikeEvent:
    public Event<
       StrikeEvent,
       EventLabelType< EventLabel::STRIKE >,
       Team,
       const sptr< Unit >&,
       const sptr< Unit >& > {
};
/*
 * 1st Arg&: const sptr<Card>& = The stunning card
 * 2nd Arg: const sptr<Unit>& = The stunned unit
 */
class StunEvent:
    public Event<
       StunEvent,
       EventLabelType< EventLabel::STUN >,
       Team,
       const sptr< Card >&,
       const sptr< Unit >& > {
};
/*
 * 1st Arg: const sptr<FieldCard>& = The summoned fieldcard
 */
class SummonEvent:
    public Event<
       SummonEvent,
       EventLabelType< EventLabel::SUMMON >,
       Team,
       const sptr< FieldCard >& > {
};
/*
 * 1st Arg: const sptr<Unit>& = The supporting unit
 * 2nd Arg: const sptr<Unit>& = The supported unit
 */
class SupportEvent:
    public Event<
       SupportEvent,
       EventLabelType< EventLabel::SUPPORT >,
       Team,
       const sptr< Unit >&,
       const sptr< Unit >& > {
};
/*
 * 1st Arg: const sptr<Card>& = The targeting card
 * 2nd Arg: const sptr<Unit>& = The targeted unit
 */
class TargetEvent:
    public Event<
       TargetEvent,
       EventLabelType< EventLabel::TARGET >,
       Team,
       const sptr< Card >&,
       const sptr< Unit >& > {
};
/*
 * 1st Arg: const sptr<Card>& = The damage causing card
 * 2nd Arg: const sptr<Unit>& = The damaged Unit
 * 3rd Arg: long = The damage amount
 */
class UnitDamageEvent:
    public Event<
       UnitDamageEvent,
       EventLabelType< EventLabel::UNIT_DAMAGE >,
       Team,
       const sptr< Card >&,
       const sptr< Unit >&,
       long > {
};


class LOREvent {
  public:
   using EventVariant = std::variant<
      AttackEvent,
      BeholdEvent,
      BlockEvent,
      CaptureEvent,
      CastEvent,
      DaybreakEvent,
      DiscardEvent,
      DrawCardEvent,
      EnlightenmentEvent,
      GainManagemEvent,
      HealUnitEvent,
      LevelUpEvent,
      NexusDamageEvent,
      NexusStrikeEvent,
      NightfallEvent,
      PlayEvent,
      RecallEvent,
      RoundEndEvent,
      RoundStartEvent,
      ScoutEvent,
      SlayEvent,
      StrikeEvent,
      StunEvent,
      SummonEvent,
      SupportEvent,
      TargetEvent,
      UnitDamageEvent >;

   LOREvent(EventVariant event) noexcept : m_event_detail(std::move(event)) {}

   template < typename DetailType >
   [[nodiscard]] inline auto& detail()
   {
      return std::get< DetailType >(m_event_detail);
   }
   template < typename DetailType >
   [[nodiscard]] inline auto& detail() const
   {
      return std::get< DetailType >(m_event_detail);
   }
   // default for not providing any template type and therefore simply getting the entire variant
   [[nodiscard]] inline auto& detail() const { return m_event_detail; }

   inline events::EventLabel label() const
   {
      return std::visit([&](const auto& event) { return event.label(); }, m_event_detail);
   }
   /**
    * This method can only be used if the caller knows precisely which Event this class is holding
    * @tparam ConcreteEventType, the exact event type present
    * @param sub, the subscriber pointer, whose type must match the ConcreteEventType's required
    * Subscriber type
    */
   template < typename ConcreteEventType >
   inline void subscribe(typename ConcreteEventType::SubscriberType* sub)
   {
      return detail< ConcreteEventType >().subscribe(sub);
   }
   inline void unsubscribe(void* sub)
   {
      std::visit([&](auto& event) { return event.unsubscribe(sub); }, m_event_detail);
   }

  private:
   EventVariant m_event_detail;
};

}  // namespace events



namespace helpers {
template < events::EventLabel label >
struct label_to_event;

template <>
struct label_to_event< events::EventLabel::ATTACK > {
   using type = events::AttackEvent;
   using label_type = events::EventLabelType< events::EventLabel::ATTACK >;
};
template <>
struct label_to_event< events::EventLabel::BEHOLD > {
   using type =events::BeholdEvent;
   using label_type = events::EventLabelType< events::EventLabel::BEHOLD >;
};
template <>
struct label_to_event< events::EventLabel::BLOCK > {
   using type =events::BlockEvent;
   using label_type = events::EventLabelType< events::EventLabel::BLOCK >;
};
template <>
struct label_to_event< events::EventLabel::CAPTURE > {
   using type =events::CaptureEvent;
   using label_type = events::EventLabelType< events::EventLabel::CAPTURE >;
};
template <>
struct label_to_event< events::EventLabel::CAST > {
   using type =events::CastEvent;
   using label_type = events::EventLabelType< events::EventLabel::CAST >;
};
template <>
struct label_to_event< events::EventLabel::DAYBREAK > {
   using type =events::DaybreakEvent;
   using label_type = events::EventLabelType< events::EventLabel::DAYBREAK >;
};

template <>
struct label_to_event< events::EventLabel::DISCARD > {
   using type =events::DiscardEvent;
   using label_type = events::EventLabelType< events::EventLabel::DISCARD >;
};
template <>
struct label_to_event< events::EventLabel::DRAW_CARD > {
   using type =events::DrawCardEvent;
   using label_type = events::EventLabelType< events::EventLabel::DRAW_CARD >;
};
template <>
struct label_to_event< events::EventLabel::ENLIGHTENMENT > {
   using type =events::EnlightenmentEvent;
   using label_type = events::EventLabelType< events::EventLabel::ENLIGHTENMENT >;
};
template <>
struct label_to_event< events::EventLabel::GAIN_MANAGEM > {
   using type =events::GainManagemEvent;
   using label_type = events::EventLabelType< events::EventLabel::GAIN_MANAGEM >;
};
template <>
struct label_to_event< events::EventLabel::HEAL_UNIT > {
   using type =events::HealUnitEvent;
   using label_type = events::EventLabelType< events::EventLabel::HEAL_UNIT >;
};
template <>
struct label_to_event< events::EventLabel::LEVEL_UP > {
   using type =events::LevelUpEvent;
   using label_type = events::EventLabelType< events::EventLabel::LEVEL_UP >;
};
template <>
struct label_to_event< events::EventLabel::NEXUS_DAMAGE > {
   using type =events::NexusDamageEvent;
   using label_type = events::EventLabelType< events::EventLabel::NEXUS_DAMAGE >;
};
template <>
struct label_to_event< events::EventLabel::NEXUS_STRIKE > {
   using type =events::NexusStrikeEvent;
   using label_type = events::EventLabelType< events::EventLabel::NEXUS_STRIKE >;
};
template <>
struct label_to_event< events::EventLabel::NIGHTFALL > {
   using type =events::NightfallEvent;
   using label_type = events::EventLabelType< events::EventLabel::NIGHTFALL >;
};
template <>
struct label_to_event< events::EventLabel::PLAY > {
   using type =events::PlayEvent;
   using label_type = events::EventLabelType< events::EventLabel::PLAY >;
};
template <>
struct label_to_event< events::EventLabel::RECALL > {
   using type =events::RecallEvent;
   using label_type = events::EventLabelType< events::EventLabel::RECALL >;
};
template <>
struct label_to_event< events::EventLabel::ROUND_END > {
   using type =events::RoundEndEvent;
   using label_type = events::EventLabelType< events::EventLabel::ROUND_END >;
};
template <>
struct label_to_event< events::EventLabel::ROUND_START > {
   using type =events::RoundStartEvent;
   using label_type = events::EventLabelType< events::EventLabel::ROUND_START >;
};
template <>
struct label_to_event< events::EventLabel::SCOUT > {
   using type =events::ScoutEvent;
   using label_type = events::EventLabelType< events::EventLabel::SCOUT >;
};
template <>
struct label_to_event< events::EventLabel::SLAY > {
   using type =events::SlayEvent;
   using label_type = events::EventLabelType< events::EventLabel::SLAY >;
};
template <>
struct label_to_event< events::EventLabel::STRIKE > {
   using type =events::StrikeEvent;
   using label_type = events::EventLabelType< events::EventLabel::STRIKE >;
};
template <>
struct label_to_event< events::EventLabel::SUMMON > {
   using type =events::SummonEvent;
   using label_type = events::EventLabelType< events::EventLabel::SUMMON >;
};
template <>
struct label_to_event< events::EventLabel::STUN > {
   using type = events::StunEvent;
   using label_type = events::EventLabelType< events::EventLabel::STUN >;
};
template <>
struct label_to_event< events::EventLabel::SUPPORT > {
   using type = events::SupportEvent;
   using label_type = events::EventLabelType< events::EventLabel::SUPPORT >;
};
template <>
struct label_to_event< events::EventLabel::TARGET > {
   using type = events::TargetEvent;
   using label_type = events::EventLabelType< events::EventLabel::TARGET >;
};
template <>
struct label_to_event< events::EventLabel::UNIT_DAMAGE > {
   using type = events::UnitDamageEvent;
   using label_type = events::EventLabelType< events::EventLabel::UNIT_DAMAGE >;
};

template < events::EventLabel elabel >
using label_to_event_t = typename label_to_event< elabel >::type;
}

#endif  // LORAINE_EVENT_TYPES_H
