
#ifndef LORAINE_EVENT_H
#define LORAINE_EVENT_H

#include <queue>
#include <set>
#include <stack>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

#include "core/gamedefs.h"
#include "core/targeting.h"
#include "event_subscriber.h"
#include "events/lor_events/event_labels.h"
#include "utils/types.h"
#include "utils/utils.h"

class GameState;

template < class Derived, class EventT, class... Args >
class EventBus: utils::CRTP< EventBus, Derived, Args...> {
  public:
   using EventData = std::tuple< EventT, Args... >;
   // classes inheriting from IEventSubscriber are the actual subscribers,
   // but we only need the crude interface
   using SubscriberType = IEventSubscriber< Derived >;
   using SubVectorType = std::vector< SubscriberType* >;

  private:
   SubVectorType subs{};

   /// SFINAE to check whether the Derived EventBus has an 'order' method of correct signature.

   template < class, class = void >
   struct has_order_method: std::false_type {
   };

   template < class T >
   struct has_order_method<
      T,
      std::void_t<
         decltype(std::declval< SubVectorType& >() = std::declval< T >().order(std::declval< Args >()...)) > >:
       std::true_type {
   };

  protected:
   void _notify(SubscriberType* subscriber, GameState& state, Args... args) {
      // passing in the EventT is needed to distinguish among ambiguous on_event overloads
      subscriber->on_event(state, EventData(EventT{}, args...));
   }

  public:
   constexpr static auto label() { return EventT::value; }
   const auto& subscribers() const { return subs; }

   void fire(GameState& state, Args... args)
   {
      // resort the subscribers according to whether the specific EventSpecialization has an
      // order method or not
      if constexpr(has_order_method< Derived >::value) {
         for(auto& sub : this->derived()->order(subs, args...)) {
            _notify(sub, state, args...);
         }
      } else {
         for(auto& sub : subs) {
            _notify(sub, state, args...);
         }
      }
   }


   void subscribe(SubscriberType* t) { subs.emplace_back(t); }

   void unsubscribe(SubscriberType* sub)
   {
      subs.erase(std::find(subs.begin(), subs.end(), sub));
   }
};

#endif  // LORAINE_EVENT_H
