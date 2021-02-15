
#ifndef LORAINE_ALL_EVENTS_H
#define LORAINE_ALL_EVENTS_H

#include "events/event.h"

// forward-declare
class Card;
class Effect;
class State;

namespace events {

class AttackEvent: public Event< Effect, State, EventType::ATTACK, Player > {
};
/*
 * 1st Arg: Card& = Card which triggers the behold effect.
 */
class BeholdEvent: public Event< Effect, State, EventType::BEHOLD, Player, Card& > {
};
class BlockEvent: public Event< Effect, State, EventType::BLOCK, Player > {
};
/*
 * 1st Arg: Card& = Card which triggered the capture
 * 2nd Arg: Target = Captor
 * 3rd Arg: Target = Captee
 */
class CaptureEvent:
    public Event< Effect, State, EventType::CAPTURE, Player, Card&, Target, Target > {
};
/*
 * 1st Arg: Card& = The casted spell
 */
class CastEvent: public Event< Effect, State, EventType::CAST, Player, Card& > {
};
/*
 * 1st ARg: Card&: The daybreak causing card
 */
class DaybreakEvent: public Event< Effect, State, EventType::DAYBREAK, Player, Card& > {
};
/*
 * 1st Arg: Card& = The killing card
 * 2nd Arg: Target = The killed card
 */
class DieEvent: public Event< Effect, State, EventType::DIE, Player, Card&, Target > {
};
/*
 * 1st Arg: Card& = The discard reason
 * 2nd Arg: Card& = The discarded card
 */
class DiscardEvent: public Event< Effect, State, EventType::DISCARD, Player, Card&, Card& > {
};
/*
 * 1st Arg: Card& = The drawn card
 */
class DrawCardEvent: public Event< Effect, State, EventType::DRAW_CARD, Player, Card& > {
};

class EnlightenmentEvent: public Event< Effect, State, EventType::ENLIGHTENMENT, Player > {
};
/*
 * 1st Arg: long = The amount of gems gained
 */
class GainManagemEvent: public Event< Effect, State, EventType::GAIN_MANAGEM, Player, long > {
};
/*
 * 1st Arg: Card& = The healing card
 * 2nd Arg: Card& = The healed card
 * 3rd Arg: long = The amount healed
 */
class HealUnitEvent:
    public Event< Effect, State, EventType::HEAL_UNIT, Player, Card&, Card&, long > {
};
/*
 * 1st Arg: Card& = The leveling champion card
 */
class LevelUpEvent: public Event< Effect, State, EventType::LEVEL_UP, Player, Card& > {
};
/*
 * 1st Arg: Card& = Nexus striking unit
 * 2nd Arg: long = The damage
 */
class NexusStrikeEvent:
    public Event< Effect, State, EventType::NEXUS_STRIKE, Player, Card&, long > {
};
/*
 * 1st Arg: Card& = The nightfall causing card
 */
class NightfallEvent: public Event< Effect, State, EventType::NIGHTFALL, Player, Card& > {
};
/*
 * 1st Arg: Card& = The played card
 */
class PlayEvent: public Event< Effect, State, EventType::ATTACK, Player, Card& > {
   void trigger(State& state, Player player, Card& played_card) override;
};
/*
 * 1st Arg: Card& = The recalling card
 * 2nd Arg: Card& = The recalled card
 */
class RecallEvent: public Event< Effect, State, EventType::RECALL, Player, Card&, Card& > {
};
/*
 * 1st Arg: long = The round number
 */
class RoundEndEvent: public Event< Effect, State, EventType::ROUND_END, Player, long > {
};
/*
 * 1st Arg: long = The round number
 */
class RoundStartEvent: public Event< Effect, State, EventType::ROUND_START, Player, long > {
};

class ScoutEvent: public Event< Effect, State, EventType::SCOUT, Player > {
};
/*
 * 1st ARg: Card& = The striking unit
 * 2nd Arg: Card& = The struck unit
 */
class StrikeEvent: public Event< Effect, State, EventType::STRIKE, Player, Card&, Card& > {
};
/*
 * 1st Arg: Card = The summoned card
 */
class SummonEvent: public Event< Effect, State, EventType::SUMMON, Player, Card& > {
};
/*
 * 1st Arg&: Card& = The stunning card
 * 2nd Arg: Card& = The stunned card
 */
class StunEvent: public Event< Effect, State, EventType::STUN, Player, Card&, Card& > {
};
/*
 * 1st Arg: Card& = The supporting card
 * 2nd Arg: Card& = The supported unit
 */
class SupportEvent: public Event< Effect, State, EventType::SUPPORT, Player, Card&, Card& > {
};
/*
 * 1st Arg: Card& = The targeting card
 * 2nd Arg: Card& = The targeted card
 */
class TargetEvent: public Event< Effect, State, EventType::TARGET, Player, Card&, Card& > {
};
/*
 * 1st Arg: Card& = The damage causing card
 * 2nd Arg: Card& = The damaged card
 * 3rd Arg: long = The damage amount
 */
class UnitTakeDamageEvent:
    public Event< Effect, State, EventType::UNIT_TAKE_DAMAGE, Player, Card&, Card&, long > {
};

template<EventType event_type>
constexpr auto construct_event()
{
   if constexpr(event_type == EventType::ATTACK) {
      return AttackEvent();
   } else if constexpr(event_type == EventType::BEHOLD) {
      return BeholdEvent();
   } else if constexpr(event_type == EventType::CAPTURE) {
      return CaptureEvent();
   } else if constexpr(event_type == EventType::CAST) {
      return CastEvent();
   } else if constexpr(event_type == EventType::BLOCK) {
      return BlockEvent();
   } else if constexpr(event_type == EventType::DAYBREAK) {
      return DaybreakEvent();
   } else if constexpr(event_type == EventType::DIE) {
      return DieEvent();
   } else if constexpr(event_type == EventType::DISCARD) {
      return DiscardEvent();
   } else if constexpr(event_type == EventType::DRAW_CARD) {
      return DrawCardEvent();
   } else if constexpr(event_type == EventType::ENLIGHTENMENT) {
      return EnlightenmentEvent();
   } else if constexpr(event_type == EventType::GAIN_MANAGEM) {
      return GainManagemEvent();
   } else if constexpr(event_type == EventType::HEAL_UNIT) {
      return HealUnitEvent();
   } else if constexpr(event_type == EventType::LEVEL_UP) {
      return LevelUpEvent();
   } else if constexpr(event_type == EventType::NEXUS_STRIKE) {
      return NexusStrikeEvent();
   } else if constexpr(event_type == EventType::NIGHTFALL) {
      return NightfallEvent();
   } else if constexpr(event_type == EventType::PLAY) {
      return PlayEvent();
   } else if constexpr(event_type == EventType::RECALL) {
      return RecallEvent();
   } else if constexpr(event_type == EventType::ROUND_END) {
      return RoundEndEvent();
   } else if constexpr(event_type == EventType::ROUND_START) {
      return RoundStartEvent();
   } else if constexpr(event_type == EventType::SCOUT) {
      return ScoutEvent();
   } else if constexpr(event_type == EventType::STRIKE) {
      return StrikeEvent();
   } else if constexpr(event_type == EventType::STUN) {
      return StunEvent();
   } else if constexpr(event_type == EventType::SUMMON) {
      return SummonEvent();
   } else if constexpr(event_type == EventType::TARGET) {
      return TargetEvent();
   } else if constexpr(event_type == EventType::UNIT_TAKE_DAMAGE) {
      return UnitTakeDamageEvent();
   }
}

}  // namespace events

#endif  // LORAINE_ALL_EVENTS_H
