
#ifndef LORAINE_EVENT_TYPES_H
#define LORAINE_EVENT_TYPES_H

#include <variant>

#include "events/event.h"

// forward-declare
class Card;
class State;

namespace events {

class AttackEvent: public Event< AttackEvent, EventLabelType< EventLabel::ATTACK >, Team > {
};
/*
 * 1st Arg: const sptr<Card>& = Card which triggers the behold effect.
 */
class BeholdEvent:
    public Event< BeholdEvent, EventLabelType< EventLabel::BEHOLD >, Team, const sptr< Card >& > {
};
class BlockEvent: public Event< BlockEvent, EventLabelType< EventLabel::BLOCK >, Team > {
};
/*
 * 1st Arg: const sptr<Card>& = Card which triggered the capture
 * 2nd Arg: Targetable& = Captor
 * 3rd Arg: Targetable& = Captee
 */
class CaptureEvent:
    public Event<
       CaptureEvent,
       EventLabelType< EventLabel::CAPTURE >,
       Team,
       const sptr< Card >&,
       Targetable&,
       Targetable& > {
};
/*
 * 1st Arg: const sptr<Card>& = The casted floating
 */
class CastEvent:
    public Event< CastEvent, EventLabelType< EventLabel::CAST >, Team, const sptr< Card >& > {
};
/*
 * 1st ARg: const sptr<Card>&: The daybreak causing card
 */
class DaybreakEvent:
    public Event<
       DaybreakEvent,
       EventLabelType< EventLabel::DAYBREAK >,
       Team,
       const sptr< Card >& > {
};
/*
 * 1st Arg: const sptr<Card>& = The killing card
 * 2nd Arg: Targetable& = The killed card
 */
class DieEvent:
    public Event<
       DieEvent,
       EventLabelType< EventLabel::DIE >,
       Team,
       const sptr< Card >&,
       Targetable& > {
};
/*
 * 1st Arg: const sptr<Card>& = The discard reason
 * 2nd Arg: const sptr<Card>& = The discarded card
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
 * 1st Arg: const sptr<Card>& = The drawn card
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
 * 2nd Arg: const sptr<Card>& = The healed card
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
 * 1st Arg: const sptr<Card>& = The leveling champion card
 */
class LevelUpEvent:
    public Event<
       LevelUpEvent,
       EventLabelType< EventLabel::LEVEL_UP >,
       Team,
       const sptr< Card >& > {
};
/*
 * 1st Arg: const sptr<Card>& = Nexus striking card
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
 * 1st Arg: const sptr<Card>& = The nightfall causing card
 */
class NightfallEvent:
    public Event<
       NightfallEvent,
       EventLabelType< EventLabel::NIGHTFALL >,
       Team,
       const sptr< Card >& > {
};
/*
 * 1st Arg: const sptr<Card>& = The played card
 */
class PlayEvent:
    public Event< PlayEvent, EventLabelType< EventLabel::ATTACK >, Team, const sptr< Card >& > {
};
/*
 * 1st Arg: const sptr<Card>& = The recalling card
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
 * 1st ARg: const sptr<Card>& = The striking common
 * 2nd Arg: const sptr<Card>& = The struck common
 */
class StrikeEvent:
    public Event<
       StrikeEvent,
       EventLabelType< EventLabel::STRIKE >,
       Team,
       const sptr< Card >&,
       const sptr< Card >& > {
};
/*
 * 1st Arg: Card = The summoned card
 */
class SummonEvent:
    public Event< SummonEvent, EventLabelType< EventLabel::SUMMON >, Team, const sptr< Card >& > {
};
/*
 * 1st Arg&: const sptr<Card>& = The stunning card
 * 2nd Arg: const sptr<Card>& = The stunned card
 */
class StunEvent:
    public Event<
       StunEvent,
       EventLabelType< EventLabel::STUN >,
       Team,
       const sptr< Card >&,
       const sptr< Card >& > {
};
/*
 * 1st Arg: const sptr<Card>& = The supporting card
 * 2nd Arg: const sptr<Card>& = The supported common
 */
class SupportEvent:
    public Event<
       SupportEvent,
       EventLabelType< EventLabel::SUPPORT >,
       Team,
       const sptr< Card >&,
       const sptr< Card >& > {
};
/*
 * 1st Arg: const sptr<Card>& = The targeting card
 * 2nd Arg: const sptr<Card>& = The targeted card
 */
class TargetEvent:
    public Event<
       TargetEvent,
       EventLabelType< EventLabel::TARGET >,
       Team,
       const sptr< Card >&,
       const sptr< Card >& > {
};
/*
 * 1st Arg: const sptr<Card>& = The damage causing card
 * 2nd Arg: const sptr<Card>& = The damaged card
 * 3rd Arg: long = The damage amount
 */
class UnitDamageEvent:
    public Event<
       UnitDamageEvent,
       EventLabelType< EventLabel::UNIT_DAMAGE >,
       Team,
       const sptr< Card >&,
       const sptr< Card >&,
       long > {
};

using LOREvent = std::variant<
   AttackEvent,
   BeholdEvent,
   BlockEvent,
   CaptureEvent,
   CastEvent,
   DaybreakEvent,
   DieEvent,
   DiscardEvent,
   DrawCardEvent,
   EnlightenmentEvent,
   GainManagemEvent,
   HealUnitEvent,
   LevelUpEvent,
   NexusStrikeEvent,
   NightfallEvent,
   PlayEvent,
   RecallEvent,
   RoundEndEvent,
   RoundStartEvent,
   ScoutEvent,
   StrikeEvent,
   SummonEvent,
   StunEvent,
   SupportEvent,
   TargetEvent,
   UnitDamageEvent >;

}  // namespace events

#endif  // LORAINE_EVENT_TYPES_H
