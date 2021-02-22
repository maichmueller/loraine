
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

template < typename... Args >
class Effect;

// an event holds a vector of subscribers
// when it fires, each is called

// our Listener will derive from EventListener<Listener>
// which holds a list of events it is subscribed to.
// As these events will have different sigs, we need a base-class.
// We will store pointers to this base-class.
class EventBase {
  public:
   virtual void unsubscribe(void* t) = 0;
};

template < class Context, events::EventType e_type, class... Args >
class Event: public EventBase {
  public:
   using SignatureTuple = std::tuple< Args... >;
   using Subscriber = Effect< Args... >;

  private:
   std::vector< Subscriber* > subscribers;

  protected:
   void _notify(Subscriber* subscriber, Context& context, Args... args)
   {
      subscriber->event_call(context, args...);
   }

  public:
   constexpr static events::EventType event_type() { return e_type; };
   const auto& get_subscribers() const { return subscribers; }

   virtual void trigger(Context& context, Args... args)
   {
      for(auto& subscriber : subscribers) {
         _notify(subscriber, context, args...);
      }
   }
   void subscribe(Subscriber* t) { subscribers.push_back(t); }

   void unsubscribe(void* t) final
   {
      auto to_remove = std::remove(
         subscribers.begin(), subscribers.end(), static_cast< Subscriber* >(t));
      subscribers.erase(to_remove, subscribers.end());
   }
};

#endif  // LORAINE_EVENT_H
