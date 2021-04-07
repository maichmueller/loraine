
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
#include "event_labels.h"
#include "eventbase.h"
#include "utils/types.h"
#include "utils/utils.h"

class GameState;

template < class Derived, class EventT, class... Args >
class Event {
  public:
   using SignatureTuple = std::tuple< EventT&, Args... >;
   // classes inheriting from EventSubscriberInterface are the actual subscribers,
   // but we only need the crude interface
   using SelfType = Event< Derived, EventT, Args... >;
   using SubscriberType = EventSubscriberInterface< SelfType >;
   using SubVectorType = std::vector< SubscriberType* >;

  private:
   SubVectorType subs;
   EventT event_ref;

   /// SFINAE to check whether the Derived Event has an 'order' method of correct signature.

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
      // passing in the event_ref is needed to distinguish among the event_call overloads
      subscriber->event_call(state, SignatureTuple(event_ref, args...));
   }

  public:
   constexpr static auto label() { return EventT::value; }
   const auto& subscribers() const { return subs; }

   void trigger(GameState& state, Args... args)
   {
      // resort the subscribers according to whether the specific EventSpecialization has an
      // order method or not
      if constexpr(has_order_method< Derived >::value) {
         for(auto& sub : this->self()->order(subs, args...)) {
            _notify(sub, state, args...);
         }
      } else {
         for(auto& sub : subs) {
            _notify(sub, state, args...);
         }
      }
   }


   void subscribe(SubscriberType* t) { subs.emplace_back(t); }

   void unsubscribe(void* t)
   {
      subs.erase(std::find(subs.begin(), subs.end(), static_cast< SubscriberType* >(t)));
   }
};

#endif  // LORAINE_EVENT_H
