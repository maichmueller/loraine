
#ifndef LORAINE_EVENT_LISTENER_H
#define LORAINE_EVENT_LISTENER_H

#include <map>
#include <queue>
#include <random>
#include <unordered_set>
#include <vector>

#include "cards/card.h"
#include "event/event.h"
#include "event_types.h"
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
   void on_event(Game& game, Player player, const AnyEvent& event);

   inline void register_card(const sptr< Card >& card)
   {
      for(const auto& [eve_type, effects] : card->get_effects_map()) {
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
      for(const auto& effect : card->get_effects_map().at(event_type)) {
         access_listeners(card->get_owner()).at(size_t(event_type)).emplace(card);
      }
   }
   void unregister_card(const sptr< Card >& card);

   inline void clear_registers(std::optional< Player > opt_player)
   {
      auto clear_listener_lambda = [&](Player player) {
         for(auto& listener : access_listeners(player))
            listener.clear();
      };
      if(opt_player.has_value()) {
         clear_listener_lambda(opt_player.value());
      } else {
         clear_listener_lambda(BLUE);
         clear_listener_lambda(RED);
      }
   }
};  // namespace events

}  // namespace events

#endif  // LORAINE_EVENT_LISTENER_H
