
#ifndef LORAINE_ALL_EVENTS_H
#define LORAINE_ALL_EVENTS_H

#include <variant>

#include "events/event.h"

// forward-declare
class Card;
class State;

namespace events {

class AttackEvent: public Event< State, EventType::ATTACK, Team > {
};
/*
 * 1st Arg: Card& = Card which triggers the behold effect.
 */
class BeholdEvent: public Event< State, EventType::BEHOLD, Team, Card& > {
};
class BlockEvent: public Event< State, EventType::BLOCK, Team > {
};
/*
 * 1st Arg: Card& = Card which triggered the capture
 * 2nd Arg: Target = Captor
 * 3rd Arg: Target = Captee
 */
class CaptureEvent: public Event< State, EventType::CAPTURE, Team, Card&, Target, Target > {
};
/*
 * 1st Arg: Card& = The casted floating
 */
class CastEvent: public Event< State, EventType::CAST, Team, Card& > {
};
/*
 * 1st ARg: Card&: The daybreak causing card
 */
class DaybreakEvent: public Event< State, EventType::DAYBREAK, Team, Card& > {
};
/*
 * 1st Arg: Card& = The killing card
 * 2nd Arg: Target = The killed card
 */
class DieEvent: public Event< State, EventType::DIE, Team, Card&, Target > {
};
/*
 * 1st Arg: Card& = The discard reason
 * 2nd Arg: Card& = The discarded card
 */
class DiscardEvent: public Event< State, EventType::DISCARD, Team, Card&, Card& > {
};
/*
 * 1st Arg: Card& = The drawn card
 */
class DrawCardEvent: public Event< State, EventType::DRAW_CARD, Team, Card& > {
};

class EnlightenmentEvent: public Event< State, EventType::ENLIGHTENMENT, Team > {
};
/*
 * 1st Arg: long = The amount of gems gained
 */
class GainManagemEvent: public Event< State, EventType::GAIN_MANAGEM, Team, long > {
};
/*
 * 1st Arg: Card& = The healing card
 * 2nd Arg: Card& = The healed card
 * 3rd Arg: long = The amount healed
 */
class HealUnitEvent: public Event< State, EventType::HEAL_UNIT, Team, Card&, Card&, long > {
};
/*
 * 1st Arg: Card& = The leveling champion card
 */
class LevelUpEvent: public Event< State, EventType::LEVEL_UP, Team, Card& > {
};
/*
 * 1st Arg: Card& = Nexus striking common
 * 2nd Arg: long = The damage
 */
class NexusStrikeEvent: public Event< State, EventType::NEXUS_STRIKE, Team, Card&, long > {
};
/*
 * 1st Arg: Card& = The nightfall causing card
 */
class NightfallEvent: public Event< State, EventType::NIGHTFALL, Team, Card& > {
};
/*
 * 1st Arg: Card& = The played card
 */
class PlayEvent: public Event< State, EventType::ATTACK, Team, Card* > {
   void trigger(State& state, Team team, Card* played_card) override;
};
/*
 * 1st Arg: Card& = The recalling card
 * 2nd Arg: Card& = The recalled card
 */
class RecallEvent: public Event< State, EventType::RECALL, Team, Card&, Card& > {
};
/*
 * 1st Arg: long = The round number
 */
class RoundEndEvent: public Event< State, EventType::ROUND_END, Team, long > {
};
/*
 * 1st Arg: long = The round number
 */
class RoundStartEvent: public Event< State, EventType::ROUND_START, Team, long > {
};

class ScoutEvent: public Event< State, EventType::SCOUT, Team > {
};
/*
 * 1st ARg: Card& = The striking common
 * 2nd Arg: Card& = The struck common
 */
class StrikeEvent: public Event< State, EventType::STRIKE, Team, Card&, Card& > {
};
/*
 * 1st Arg: Card = The summoned card
 */
class SummonEvent: public Event< State, EventType::SUMMON, Team, Card& > {
};
/*
 * 1st Arg&: Card& = The stunning card
 * 2nd Arg: Card& = The stunned card
 */
class StunEvent: public Event< State, EventType::STUN, Team, Card&, Card& > {
};
/*
 * 1st Arg: Card& = The supporting card
 * 2nd Arg: Card& = The supported common
 */
class SupportEvent: public Event< State, EventType::SUPPORT, Team, Card&, Card& > {
};
/*
 * 1st Arg: Card& = The targeting card
 * 2nd Arg: Card& = The targeted card
 */
class TargetEvent: public Event< State, EventType::TARGET, Team, Card&, Card& > {
};
/*
 * 1st Arg: Card& = The damage causing card
 * 2nd Arg: Card& = The damaged card
 * 3rd Arg: long = The damage amount
 */
class UnitDamageEvent: public Event< State, EventType::UNIT_DAMAGE, Team, Card&, Card&, long > {
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

#endif  // LORAINE_ALL_EVENTS_H
