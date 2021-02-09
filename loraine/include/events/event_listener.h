
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

class Game;

namespace events {

class EventListener {
  public:
   using ListenerType = std::unordered_set< sptr< Card > >;
   using ListenerArrType = std::array< ListenerType, n_events >;
   using ListenerQueueType = std::queue< sptr< Card > >;

  private:
   SymArr< ListenerArrType > m_listeners;

  public:
   template < typename T >
   auto& access_listeners(const T& accessor)
   {
      return m_listeners[accessor];
   }
   template < typename T >
   auto& access_listeners(const T& accessor) const
   {
      return m_listeners[accessor];
   }
   void on_event(State& state, const AnyEvent& event);

   inline void register_card(const sptr< Card >& card)
   {
      for(const auto& [eve_type, effects] : card->get_mutable_attrs().effects) {
         bool has_unconsumed_effects = false;
         for(const auto& effect : effects) {
            if(not effect.is_consumed()) {
               has_unconsumed_effects = true;
               break;
            }
         }
         if(has_unconsumed_effects) {
            register_effects_for_event(card, eve_type);
         }
      }
   }
   inline void register_effects_for_event(const sptr< Card >& card, EventType event_type)
   {
      for(const auto& effect : card->get_effects(event_type)) {
         access_listeners(card->get_mutable_attrs().owner).at(size_t(event_type)).emplace(card);
      }
   }
   void unregister_card(const sptr< Card >& card);

   inline void clear_registers(std::optional< Player > opt_player)
   {
      auto clear_listener_lambda = [&](Player player) {
         for(auto& listener : access_listeners(player)) {
            listener.clear();
         }
      };
      if(opt_player.has_value()) {
         clear_listener_lambda(opt_player.value());
      } else {
         clear_listener_lambda(BLUE);
         clear_listener_lambda(RED);
      }
   }
   template < events::EventType event_type, typename... Params >
   constexpr inline void trigger_event(Params... params)
   {
      using namespace events;
      if constexpr(event_type == EventType::NONE) {
         _trigger_event(NoneEvent(std::forward< Params... >(params...)));
      } else if constexpr(event_type == EventType::ATTACK) {
         _trigger_event(AttackEvent(std::forward< Params... >(params...)));
      } else if constexpr(event_type == EventType::BEHOLD) {
         _trigger_event(BeholdEvent(std::forward< Params... >(params...)));
      } else if constexpr(event_type == EventType::CAPTURE) {
         _trigger_event(CaptureEvent(std::forward< Params... >(params...)));
      } else if constexpr(event_type == EventType::CAST) {
         _trigger_event(CastEvent(std::forward< Params... >(params...)));
      } else if constexpr(event_type == EventType::BLOCK) {
         _trigger_event(BlockEvent(std::forward< Params... >(params...)));
      } else if constexpr(event_type == EventType::DAYBREAK) {
         _trigger_event(DaybreakEvent(std::forward< Params... >(params...)));
      } else if constexpr(event_type == EventType::DIE) {
         _trigger_event(DieEvent(std::forward< Params... >(params...)));
      } else if constexpr(event_type == EventType::DISCARD) {
         _trigger_event(DiscardEvent(std::forward< Params... >(params...)));
      } else if constexpr(event_type == EventType::DRAW_CARD) {
         _trigger_event(DrawCardEvent(std::forward< Params... >(params...)));
      } else if constexpr(event_type == EventType::ENLIGHTENMENT) {
         _trigger_event(EnlightenmentEvent(std::forward< Params... >(params...)));
      } else if constexpr(event_type == EventType::GAIN_MANAGEM) {
         _trigger_event(GainManagemEvent(std::forward< Params... >(params...)));
      } else if constexpr(event_type == EventType::HEAL_UNIT) {
         _trigger_event(HealUnitEvent(std::forward< Params... >(params...)));
      } else if constexpr(event_type == EventType::LEVEL_UP) {
         _trigger_event(LevelUpEvent(std::forward< Params... >(params...)));
      } else if constexpr(event_type == EventType::NEXUS_STRIKE) {
         _trigger_event(NexusStrikeEvent(std::forward< Params... >(params...)));
      } else if constexpr(event_type == EventType::NIGHTFALL) {
         _trigger_event(NightfallEvent(std::forward< Params... >(params...)));
      } else if constexpr(event_type == EventType::PLAY) {
         _trigger_event(PlayEvent(std::forward< Params... >(params...)));
      } else if constexpr(event_type == EventType::RECALL) {
         _trigger_event(RecallEvent(std::forward< Params... >(params...)));
      } else if constexpr(event_type == EventType::ROUND_END) {
         _trigger_event(RoundEndEvent(std::forward< Params... >(params...)));
      } else if constexpr(event_type == EventType::ROUND_START) {
         _trigger_event(RoundStartEvent(std::forward< Params... >(params...)));
      } else if constexpr(event_type == EventType::SCOUT) {
         _trigger_event(ScoutEvent(std::forward< Params... >(params...)));
      } else if constexpr(event_type == EventType::STRIKE) {
         _trigger_event(StrikeEvent(std::forward< Params... >(params...)));
      } else if constexpr(event_type == EventType::STUN) {
         _trigger_event(StunEvent(std::forward< Params... >(params...)));
      } else if constexpr(event_type == EventType::SUMMON) {
         _trigger_event(SummonEvent(std::forward< Params... >(params...)));
      } else if constexpr(event_type == EventType::TARGET) {
         _trigger_event(TargetEvent(std::forward< Params... >(params...)));
      } else if constexpr(event_type == EventType::UNIT_TAKE_DAMAGE) {
         _trigger_event(UnitTakeDamageEvent(std::forward< Params... >(params...)));
      }
   }
};

}  // namespace events

#endif  // LORAINE_EVENT_LISTENER_H
