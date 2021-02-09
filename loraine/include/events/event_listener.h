
#ifndef LORAINE_EVENT_LISTENER_H
#define LORAINE_EVENT_LISTENER_H

#include <map>
#include <queue>
#include <random>
#include <unordered_set>
#include <vector>

#include "cards/card.h"
#include "event_types.h"
#include "events/event.h"
#include "types.h"

class State;

namespace events {

class EventListener {
  public:
   // we use raw pointers for the cards here, since the EventListener is supposed to
   // be a member of the State object. As such, all cards in this state will be deleted
   // when the State is deleted and the listener doesn't have to care about that.

   // This also implies that every deleted card object has to be properly unsubscribed
   // from the listener!

   using ListenerType = std::unordered_set< Card* >;
   using ListenerArrType = std::array< ListenerType, n_events >;

   auto& listener(size_t index) { return m_listener[index]; }
   const auto& listener(size_t index) const { return m_listener[index]; }
   auto& listener(const events::EventType& et) { return m_listener[static_cast< size_t >(et)]; }
   const auto& listener(const events::EventType& et) const
   {
      return m_listener[static_cast< size_t >(et)];
   }

   template < events::EventType event_type, typename... Params >
   void on_event(State& state, Params... params);

   inline void subscribe(Card& card)
   {
      for(const auto& [eve_type, effects] : card.get_mutable_attrs().effects) {
         for(const auto& effect : effects) {
            if(not effect.is_consumed()) {
               listener(eve_type).emplace(&card);
               break;
            }
         }
      }
   }
   void unsubscribe(Card& card);

   inline void clear_registers()
   {
      for(auto& listener : m_listener) {
         listener.clear();
      }
   }

  private:
   ListenerArrType m_listener;

   template < events::EventType event_type, typename... Params >
   AnyEvent _build_event(Params... params);
};

template < events::EventType event_type, typename... Params >
void EventListener::on_event(State& state, Params... params)
{
   // trigger the event construction first
   auto event = _build_event< event_type >(std::forward< Params >(params)...);
   const auto& registered_cards = listener(event->event_type());
   // trigger every card that is subscribed to this event
   for(const auto& card : registered_cards) {
      card(state, event);
   }
}

template < events::EventType event_type, typename... Params >
AnyEvent EventListener::_build_event(Params... params)
{
   using namespace events;
   if constexpr(event_type == EventType::ATTACK) {
      return AttackEvent(std::forward< Params... >(params...));
   } else if constexpr(event_type == EventType::BEHOLD) {
      return BeholdEvent(std::forward< Params... >(params...));
   } else if constexpr(event_type == EventType::CAPTURE) {
      return CaptureEvent(std::forward< Params... >(params...));
   } else if constexpr(event_type == EventType::CAST) {
      return CastEvent(std::forward< Params... >(params...));
   } else if constexpr(event_type == EventType::BLOCK) {
      return BlockEvent(std::forward< Params... >(params...));
   } else if constexpr(event_type == EventType::DAYBREAK) {
      return DaybreakEvent(std::forward< Params... >(params...));
   } else if constexpr(event_type == EventType::DIE) {
      return DieEvent(std::forward< Params... >(params...));
   } else if constexpr(event_type == EventType::DISCARD) {
      return DiscardEvent(std::forward< Params... >(params...));
   } else if constexpr(event_type == EventType::DRAW_CARD) {
      return DrawCardEvent(std::forward< Params... >(params...));
   } else if constexpr(event_type == EventType::ENLIGHTENMENT) {
      return EnlightenmentEvent(std::forward< Params... >(params...));
   } else if constexpr(event_type == EventType::GAIN_MANAGEM) {
      return GainManagemEvent(std::forward< Params... >(params...));
   } else if constexpr(event_type == EventType::HEAL_UNIT) {
      return HealUnitEvent(std::forward< Params... >(params...));
   } else if constexpr(event_type == EventType::LEVEL_UP) {
      return LevelUpEvent(std::forward< Params... >(params...));
   } else if constexpr(event_type == EventType::NEXUS_STRIKE) {
      return NexusStrikeEvent(std::forward< Params... >(params...));
   } else if constexpr(event_type == EventType::NIGHTFALL) {
      return NightfallEvent(std::forward< Params... >(params...));
   } else if constexpr(event_type == EventType::PLAY) {
      return PlayEvent(std::forward< Params... >(params...));
   } else if constexpr(event_type == EventType::RECALL) {
      return RecallEvent(std::forward< Params... >(params...));
   } else if constexpr(event_type == EventType::ROUND_END) {
      return RoundEndEvent(std::forward< Params... >(params...));
   } else if constexpr(event_type == EventType::ROUND_START) {
      return RoundStartEvent(std::forward< Params... >(params...));
   } else if constexpr(event_type == EventType::SCOUT) {
      return ScoutEvent(std::forward< Params... >(params...));
   } else if constexpr(event_type == EventType::STRIKE) {
      return StrikeEvent(std::forward< Params... >(params...));
   } else if constexpr(event_type == EventType::STUN) {
      return StunEvent(std::forward< Params... >(params...));
   } else if constexpr(event_type == EventType::SUMMON) {
      return SummonEvent(std::forward< Params... >(params...));
   } else if constexpr(event_type == EventType::TARGET) {
      return TargetEvent(std::forward< Params... >(params...));
   } else if constexpr(event_type == EventType::UNIT_TAKE_DAMAGE) {
      return UnitTakeDamageEvent(std::forward< Params... >(params...));
   }
}

}  // namespace events

#endif  // LORAINE_EVENT_LISTENER_H
