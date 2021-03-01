
#ifndef LORAINE_EVENT_TYPES_H
#define LORAINE_EVENT_TYPES_H

#include <variant>

#include "events/event.h"

// forward-declare
class Card;
class State;

namespace events {

class AttackEvent: public Event< EventLabelType< EventLabel::ATTACK >, Team > {
};
/*
 * 1st Arg: Card& = Card which triggers the behold effect.
 */
class BeholdEvent: public Event< EventLabelType< EventLabel::BEHOLD >, Team, Card& > {
};
class BlockEvent: public Event< EventLabelType< EventLabel::BLOCK >, Team > {
};
/*
 * 1st Arg: Card& = Card which triggered the capture
 * 2nd Arg: Target = Captor
 * 3rd Arg: Target = Captee
 */
class CaptureEvent:
    public Event< EventLabelType< EventLabel::CAPTURE >, Team, Card&, Target, Target > {
};
/*
 * 1st Arg: Card& = The casted floating
 */
class CastEvent: public Event< EventLabelType< EventLabel::CAST >, Team, Card& > {
};
/*
 * 1st ARg: Card&: The daybreak causing card
 */
class DaybreakEvent: public Event< EventLabelType< EventLabel::DAYBREAK >, Team, Card& > {
};
/*
 * 1st Arg: Card& = The killing card
 * 2nd Arg: Target = The killed card
 */
class DieEvent: public Event< EventLabelType< EventLabel::DIE >, Team, Card&, Target > {
};
/*
 * 1st Arg: Card& = The discard reason
 * 2nd Arg: Card& = The discarded card
 */
class DiscardEvent: public Event< EventLabelType< EventLabel::DISCARD >, Team, Card&, Card& > {
};
/*
 * 1st Arg: Card& = The drawn card
 */
class DrawCardEvent: public Event< EventLabelType< EventLabel::DRAW_CARD >, Team, Card& > {
};

class EnlightenmentEvent: public Event< EventLabelType< EventLabel::ENLIGHTENMENT >, Team > {
};
/*
 * 1st Arg: long = The amount of gems gained
 */
class GainManagemEvent: public Event< EventLabelType< EventLabel::GAIN_MANAGEM >, Team, long > {
};
/*
 * 1st Arg: Card& = The healing card
 * 2nd Arg: Card& = The healed card
 * 3rd Arg: long = The amount healed
 */
class HealUnitEvent:
    public Event< EventLabelType< EventLabel::HEAL_UNIT >, Team, Card&, Card&, long > {
};
/*
 * 1st Arg: Card& = The leveling champion card
 */
class LevelUpEvent: public Event< EventLabelType< EventLabel::LEVEL_UP >, Team, Card& > {
};
/*
 * 1st Arg: Card& = Nexus striking common
 * 2nd Arg: long = The damage
 */
class NexusStrikeEvent:
    public Event< EventLabelType< EventLabel::NEXUS_STRIKE >, Team, Card&, long > {
};
/*
 * 1st Arg: Card& = The nightfall causing card
 */
class NightfallEvent: public Event< EventLabelType< EventLabel::NIGHTFALL >, Team, Card& > {
};
/*
 * 1st Arg: Card& = The played card
 */
class PlayEvent: public Event< EventLabelType< EventLabel::ATTACK >, Team, Card* > {
};
/*
 * 1st Arg: Card& = The recalling card
 * 2nd Arg: Card& = The recalled card
 */
class RecallEvent: public Event< EventLabelType< EventLabel::RECALL >, Team, Card&, Card& > {
};
/*
 * 1st Arg: long = The round number
 */
class RoundEndEvent: public Event< EventLabelType< EventLabel::ROUND_END >, Team, long > {
};
/*
 * 1st Arg: long = The round number
 */
class RoundStartEvent: public Event< EventLabelType< EventLabel::ROUND_START >, Team, long > {
};

class ScoutEvent: public Event< EventLabelType< EventLabel::SCOUT >, Team > {
};
/*
 * 1st ARg: Card& = The striking common
 * 2nd Arg: Card& = The struck common
 */
class StrikeEvent: public Event< EventLabelType< EventLabel::STRIKE >, Team, Card&, Card& > {
};
/*
 * 1st Arg: Card = The summoned card
 */
class SummonEvent: public Event< EventLabelType< EventLabel::SUMMON >, Team, Card& > {
};
/*
 * 1st Arg&: Card& = The stunning card
 * 2nd Arg: Card& = The stunned card
 */
class StunEvent: public Event< EventLabelType< EventLabel::STUN >, Team, Card&, Card& > {
};
/*
 * 1st Arg: Card& = The supporting card
 * 2nd Arg: Card& = The supported common
 */
class SupportEvent: public Event< EventLabelType< EventLabel::SUPPORT >, Team, Card&, Card& > {
};
/*
 * 1st Arg: Card& = The targeting card
 * 2nd Arg: Card& = The targeted card
 */
class TargetEvent: public Event< EventLabelType< EventLabel::TARGET >, Team, Card&, Card& > {
};
/*
 * 1st Arg: Card& = The damage causing card
 * 2nd Arg: Card& = The damaged card
 * 3rd Arg: long = The damage amount
 */
class UnitDamageEvent:
    public Event< EventLabelType< EventLabel::UNIT_DAMAGE >, Team, Card&, Card&, long > {
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
