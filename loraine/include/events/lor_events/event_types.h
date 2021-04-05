
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

template < EventLabel label >
struct label_to_event_helper;

template <>
struct label_to_event_helper< EventLabel::ATTACK > {
   using type = AttackEvent;
   using label_type = EventLabelType< EventLabel::ATTACK >;
};
template <>
struct label_to_event_helper< EventLabel::BEHOLD > {
   using type = BeholdEvent;
   using label_type = EventLabelType< EventLabel::BEHOLD >;
};
template <>
struct label_to_event_helper< EventLabel::BLOCK > {
   using type = BlockEvent;
   using label_type = EventLabelType< EventLabel::BLOCK >;
};
template <>
struct label_to_event_helper< EventLabel::CAPTURE > {
   using type = CaptureEvent;
   using label_type = EventLabelType< EventLabel::CAPTURE >;
};
template <>
struct label_to_event_helper< EventLabel::CAST > {
   using type = CastEvent;
   using label_type = EventLabelType< EventLabel::CAST >;
};
template <>
struct label_to_event_helper< EventLabel::DAYBREAK > {
   using type = DaybreakEvent;
   using label_type = EventLabelType< EventLabel::DAYBREAK >;
};

template <>
struct label_to_event_helper< EventLabel::DISCARD > {
   using type = DiscardEvent;
   using label_type = EventLabelType< EventLabel::DISCARD >;
};
template <>
struct label_to_event_helper< EventLabel::DRAW_CARD > {
   using type = DrawCardEvent;
   using label_type = EventLabelType< EventLabel::DRAW_CARD >;
};
template <>
struct label_to_event_helper< EventLabel::ENLIGHTENMENT > {
   using type = EnlightenmentEvent;
   using label_type = EventLabelType< EventLabel::ENLIGHTENMENT >;
};
template <>
struct label_to_event_helper< EventLabel::GAIN_MANAGEM > {
   using type = GainManagemEvent;
   using label_type = EventLabelType< EventLabel::GAIN_MANAGEM >;
};
template <>
struct label_to_event_helper< EventLabel::HEAL_UNIT > {
   using type = HealUnitEvent;
   using label_type = EventLabelType< EventLabel::HEAL_UNIT >;
};
template <>
struct label_to_event_helper< EventLabel::LEVEL_UP > {
   using type = LevelUpEvent;
   using label_type = EventLabelType< EventLabel::LEVEL_UP >;
};
template <>
struct label_to_event_helper< EventLabel::NEXUS_DAMAGE > {
   using type = NexusDamageEvent;
   using label_type = EventLabelType< EventLabel::NEXUS_DAMAGE >;
};
template <>
struct label_to_event_helper< EventLabel::NEXUS_STRIKE > {
   using type = NexusStrikeEvent;
   using label_type = EventLabelType< EventLabel::NEXUS_STRIKE >;
};
template <>
struct label_to_event_helper< EventLabel::NIGHTFALL > {
   using type = NightfallEvent;
   using label_type = EventLabelType< EventLabel::NIGHTFALL >;
};
template <>
struct label_to_event_helper< EventLabel::PLAY > {
   using type = PlayEvent;
   using label_type = EventLabelType< EventLabel::PLAY >;
};
template <>
struct label_to_event_helper< EventLabel::RECALL > {
   using type = RecallEvent;
   using label_type = EventLabelType< EventLabel::RECALL >;
};
template <>
struct label_to_event_helper< EventLabel::ROUND_END > {
   using type = RoundEndEvent;
   using label_type = EventLabelType< EventLabel::ROUND_END >;
};
template <>
struct label_to_event_helper< EventLabel::ROUND_START > {
   using type = RoundStartEvent;
   using label_type = EventLabelType< EventLabel::ROUND_START >;
};
template <>
struct label_to_event_helper< EventLabel::SCOUT > {
   using type = ScoutEvent;
   using label_type = EventLabelType< EventLabel::SCOUT >;
};
template <>
struct label_to_event_helper< EventLabel::SLAY > {
   using type = SlayEvent;
   using label_type = EventLabelType< EventLabel::SLAY >;
};
template <>
struct label_to_event_helper< EventLabel::STRIKE > {
   using type = StrikeEvent;
   using label_type = EventLabelType< EventLabel::STRIKE >;
};
template <>
struct label_to_event_helper< EventLabel::SUMMON > {
   using type = SummonEvent;
   using label_type = EventLabelType< EventLabel::SUMMON >;
};
template <>
struct label_to_event_helper< EventLabel::STUN > {
   using type = StunEvent;
   using label_type = EventLabelType< EventLabel::STUN >;
};
template <>
struct label_to_event_helper< EventLabel::SUPPORT > {
   using type = SupportEvent;
   using label_type = EventLabelType< EventLabel::SUPPORT >;
};
template <>
struct label_to_event_helper< EventLabel::TARGET > {
   using type = TargetEvent;
   using label_type = EventLabelType< EventLabel::TARGET >;
};
template <>
struct label_to_event_helper< EventLabel::UNIT_DAMAGE > {
   using type = UnitDamageEvent;
   using label_type = EventLabelType< EventLabel::UNIT_DAMAGE >;
};

template < EventLabel elabel >
using label_to_event_helper_t = typename label_to_event_helper< elabel >::type;

class LOREvent {
  public:
   using EventVariant = std::variant<
      AttackEvent,
      BeholdEvent,
      BlockEvent,
      CaptureEvent,
      CastEvent,
      DaybreakEvent,
      SlayEvent,
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
      StrikeEvent,
      StunEvent,
      SummonEvent,
      SupportEvent,
      TargetEvent,
      UnitDamageEvent >;

   explicit LOREvent(EventVariant event) noexcept : m_event_detail(std::move(event)) {}

   template < typename DetailType >
   [[nodiscard]] inline auto detail() const
   {
      return std::get< DetailType >(m_event_detail);
   }
   // default for not providing any template type and therefore simply getting the entire variant
   [[nodiscard]] inline auto detail() const { return m_event_detail; }

   inline bool execute(GameState& state)
   {
      return std::visit([&](auto& action) { return action.execute(state); }, m_action_detail);
   }
   [[nodiscard]] bool is_accept() const { return label() == ActionLabel::ACCEPT; }
   [[nodiscard]] bool is_cancellation() const { return label() == ActionLabel::CANCEL; }
   [[nodiscard]] bool is_choice() const { return label() == ActionLabel::CHOICE; }
   [[nodiscard]] bool is_dragging_enemy() const { return label() == ActionLabel::DRAG_ENEMY; }
   [[nodiscard]] bool is_mulligan() const { return label() == ActionLabel::MULLIGAN; }
   [[nodiscard]] bool is_placing_spell() const { return label() == ActionLabel::PLACE_SPELL; }
   [[nodiscard]] bool is_placing_unit() const { return label() == ActionLabel::PLACE_UNIT; }
   [[nodiscard]] bool is_play_finish() const { return label() == ActionLabel::PLAY_FINISH; }
   [[nodiscard]] bool is_play_request() const { return label() == ActionLabel::PLAY_REQUEST; }
   [[nodiscard]] bool is_targeting() const { return label() == ActionLabel::TARGETING; }

  private:
   EventVariant m_event_detail;
};

}  // namespace events

#endif  // LORAINE_EVENT_TYPES_H
