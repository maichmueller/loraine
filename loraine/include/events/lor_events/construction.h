
#ifndef LORAINE_CONSTRUCTION_H
#define LORAINE_CONSTRUCTION_H

#include "all_events.h"
#include "cards/effect.h"

template < EventType event_type >
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
   } else if constexpr(event_type == EventType::UNIT_DAMAGE) {
      return UnitDamageEvent();
   }
}

template <size_t e>
void event_array(std::array<uptr<EventBase>, n_events>& arr) {
   arr[e] = std::make_unique(construct_event<static_cast<EventType>(e)>());
   event_array<e-1>(arr);
}

template <>
void event_array<0>(std::array<uptr<EventBase>, n_events>& arr) {
   arr[0] = std::make_unique(construct_event<static_cast<EventType>(0)>());
}

#endif  // LORAINE_CONSTRUCTION_H
