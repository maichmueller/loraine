
#ifndef LORAINE_EVENT_H
#define LORAINE_EVENT_H

#include <stack>
#include <utility>
#include <variant>
#include <vector>

#include "engine/gamedefs.h"
#include "event_types.h"
#include "target.h"
#include "utils/types.h"
#include "utils/utils.h"

template < typename... Args >
class Effect;

// an event holds a vector of subs
// when it fires, each is called

// our Listener will derive from EventListener<Listener>
// which holds a list of events it is subscribed to.
// As these events will have different sigs, we need a base-class.
// We will store pointers to this base-class.
class EventBase {
  public:
   virtual void unsubscribe(void* t) = 0;
};


template < class Derived, class Context, events::EventType e_type, class... Args >
class Event: public EventBase, public CRTP< Event, Derived > {
  public:
   using SignatureTuple = std::tuple< Args... >;
   using Subscriber = Effect< Args... >;

  private:
   std::vector< Subscriber* > subs;

  protected:
   void _notify(Subscriber* subscriber, Context& context, Args... args)
   {
      std::get< 0 >(std::make_tuple(3, 4.5));
      subscriber->event_call(context, args...);
   }

  public:
   constexpr static events::EventType event_type() { return e_type; };
   const auto& subscribers() const { return subs; }

   virtual void trigger(Context& context, Args... args)
   {
      for(auto& sub : order(subs)) {
         _notify(sub, context, args...);
      }
   }
   void subscribe(Subscriber* t) { subs.push_back(t); }

   void unsubscribe(void* t) final
   {
      auto to_remove = std::remove(subs.begin(), subs.end(), static_cast< Subscriber* >(t));
      subs.erase(to_remove, subs.end());
   }
   std::vector< Subscriber* > order(const std::vector< Subscriber* >& subs)
   {
      return static_cast< Derived* >(this)->order_impl(subs);
   }
};

#endif  // LORAINE_EVENT_H
