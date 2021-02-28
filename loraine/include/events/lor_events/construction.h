
#ifndef LORAINE_CONSTRUCTION_H
#define LORAINE_CONSTRUCTION_H

#include "all_events.h"
#include "cards/effect.h"

namespace events {
template < EventLabel event_type >
constexpr auto construct_event()
{
   if constexpr(event_type == EventLabel::ATTACK) {
      return std::make_unique< AttackEvent >();
   } else if constexpr(event_type == EventLabel::BEHOLD) {
      return std::make_unique< BeholdEvent >();
   } else if constexpr(event_type == EventLabel::CAPTURE) {
      return std::make_unique< CaptureEvent >();
   } else if constexpr(event_type == EventLabel::CAST) {
      return std::make_unique< CastEvent >();
   } else if constexpr(event_type == EventLabel::BLOCK) {
      return std::make_unique< BlockEvent >();
   } else if constexpr(event_type == EventLabel::DAYBREAK) {
      return std::make_unique< DaybreakEvent >();
   } else if constexpr(event_type == EventLabel::DIE) {
      return std::make_unique< DieEvent >();
   } else if constexpr(event_type == EventLabel::DISCARD) {
      return std::make_unique< DiscardEvent >();
   } else if constexpr(event_type == EventLabel::DRAW_CARD) {
      return std::make_unique< DrawCardEvent >();
   } else if constexpr(event_type == EventLabel::ENLIGHTENMENT) {
      return std::make_unique< EnlightenmentEvent >();
   } else if constexpr(event_type == EventLabel::GAIN_MANAGEM) {
      return std::make_unique< GainManagemEvent >();
   } else if constexpr(event_type == EventLabel::HEAL_UNIT) {
      return std::make_unique< HealUnitEvent >();
   } else if constexpr(event_type == EventLabel::LEVEL_UP) {
      return std::make_unique< LevelUpEvent >();
   } else if constexpr(event_type == EventLabel::NEXUS_STRIKE) {
      return std::make_unique< NexusStrikeEvent >();
   } else if constexpr(event_type == EventLabel::NIGHTFALL) {
      return std::make_unique< NightfallEvent >();
   } else if constexpr(event_type == EventLabel::PLAY) {
      return std::make_unique< PlayEvent >();
   } else if constexpr(event_type == EventLabel::RECALL) {
      return std::make_unique< RecallEvent >();
   } else if constexpr(event_type == EventLabel::ROUND_END) {
      return std::make_unique< RoundEndEvent >();
   } else if constexpr(event_type == EventLabel::ROUND_START) {
      return std::make_unique< RoundStartEvent >();
   } else if constexpr(event_type == EventLabel::SCOUT) {
      return std::make_unique< ScoutEvent >();
   } else if constexpr(event_type == EventLabel::STRIKE) {
      return std::make_unique< StrikeEvent >();
   } else if constexpr(event_type == EventLabel::STUN) {
      return std::make_unique< StunEvent >();
   } else if constexpr(event_type == EventLabel::SUMMON) {
      return std::make_unique< SummonEvent >();
   } else if constexpr(event_type == EventLabel::TARGET) {
      return std::make_unique< TargetEvent >();
   } else if constexpr(event_type == EventLabel::UNIT_DAMAGE) {
      return std::make_unique< UnitDamageEvent >();
   }
}

template < size_t e >
void fill_event_array(std::array< uptr< EventBase >, n_events >& arr)
{
   arr[e] = construct_event< static_cast< EventLabel >(e) >();
   fill_event_array< e - 1 >(arr);
}

template <>
void fill_event_array< 0 >(std::array< uptr< EventBase >, n_events >& arr)
{
   arr[0] = construct_event< static_cast< EventLabel >(0) >();
}

}
#endif  // LORAINE_CONSTRUCTION_H
