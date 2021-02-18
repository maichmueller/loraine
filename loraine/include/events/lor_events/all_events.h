
#ifndef LORAINE_ALL_EVENTS_H
#define LORAINE_ALL_EVENTS_H

#include "events/event.h"

// forward-declare
class Card;
class State;

namespace events {

class AttackEvent: public Event< State, EventType::ATTACK, Player > {
};
/*
 * 1st Arg: Card& = Card which triggers the behold effect.
 */
class BeholdEvent: public Event< State, EventType::BEHOLD, Player, Card& > {
};
class BlockEvent: public Event< State, EventType::BLOCK, Player > {
};
/*
 * 1st Arg: Card& = Card which triggered the capture
 * 2nd Arg: Target = Captor
 * 3rd Arg: Target = Captee
 */
class CaptureEvent: public Event< State, EventType::CAPTURE, Player, Card&, Target, Target > {
};
/*
 * 1st Arg: Card& = The casted spell
 */
class CastEvent: public Event< State, EventType::CAST, Player, Card& > {
};
/*
 * 1st ARg: Card&: The daybreak causing card
 */
class DaybreakEvent: public Event< State, EventType::DAYBREAK, Player, Card& > {
};
/*
 * 1st Arg: Card& = The killing card
 * 2nd Arg: Target = The killed card
 */
class DieEvent: public Event< State, EventType::DIE, Player, Card&, Target > {
};
/*
 * 1st Arg: Card& = The discard reason
 * 2nd Arg: Card& = The discarded card
 */
class DiscardEvent: public Event< State, EventType::DISCARD, Player, Card&, Card& > {
};
/*
 * 1st Arg: Card& = The drawn card
 */
class DrawCardEvent: public Event< State, EventType::DRAW_CARD, Player, Card& > {
};

class EnlightenmentEvent: public Event< State, EventType::ENLIGHTENMENT, Player > {
};
/*
 * 1st Arg: long = The amount of gems gained
 */
class GainManagemEvent: public Event< State, EventType::GAIN_MANAGEM, Player, long > {
};
/*
 * 1st Arg: Card& = The healing card
 * 2nd Arg: Card& = The healed card
 * 3rd Arg: long = The amount healed
 */
class HealUnitEvent: public Event< State, EventType::HEAL_UNIT, Player, Card&, Card&, long > {
};
/*
 * 1st Arg: Card& = The leveling champion card
 */
class LevelUpEvent: public Event< State, EventType::LEVEL_UP, Player, Card& > {
};
/*
 * 1st Arg: Card& = Nexus striking unit
 * 2nd Arg: long = The damage
 */
class NexusStrikeEvent: public Event< State, EventType::NEXUS_STRIKE, Player, Card&, long > {
};
/*
 * 1st Arg: Card& = The nightfall causing card
 */
class NightfallEvent: public Event< State, EventType::NIGHTFALL, Player, Card& > {
};
/*
 * 1st Arg: Card& = The played card
 */
class PlayEvent: public Event< State, EventType::ATTACK, Player, Card* > {
   void trigger(State& state, Player player, Card* played_card) override;
};
/*
 * 1st Arg: Card& = The recalling card
 * 2nd Arg: Card& = The recalled card
 */
class RecallEvent: public Event< State, EventType::RECALL, Player, Card&, Card& > {
};
/*
 * 1st Arg: long = The round number
 */
class RoundEndEvent: public Event< State, EventType::ROUND_END, Player, long > {
};
/*
 * 1st Arg: long = The round number
 */
class RoundStartEvent: public Event< State, EventType::ROUND_START, Player, long > {
};

class ScoutEvent: public Event< State, EventType::SCOUT, Player > {
};
/*
 * 1st ARg: Card& = The striking unit
 * 2nd Arg: Card& = The struck unit
 */
class StrikeEvent: public Event< State, EventType::STRIKE, Player, Card&, Card& > {
};
/*
 * 1st Arg: Card = The summoned card
 */
class SummonEvent: public Event< State, EventType::SUMMON, Player, Card& > {
};
/*
 * 1st Arg&: Card& = The stunning card
 * 2nd Arg: Card& = The stunned card
 */
class StunEvent: public Event< State, EventType::STUN, Player, Card&, Card& > {
};
/*
 * 1st Arg: Card& = The supporting card
 * 2nd Arg: Card& = The supported unit
 */
class SupportEvent: public Event< State, EventType::SUPPORT, Player, Card&, Card& > {
};
/*
 * 1st Arg: Card& = The targeting card
 * 2nd Arg: Card& = The targeted card
 */
class TargetEvent: public Event< State, EventType::TARGET, Player, Card&, Card& > {
};
/*
 * 1st Arg: Card& = The damage causing card
 * 2nd Arg: Card& = The damaged card
 * 3rd Arg: long = The damage amount
 */
class UnitDamageEvent:
    public Event< State, EventType::UNIT_DAMAGE, Player, Card&, Card&, long > {
};


}  // namespace events

#endif  // LORAINE_ALL_EVENTS_H
