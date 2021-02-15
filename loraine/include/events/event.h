
#ifndef LORAINE_EVENT_H
#define LORAINE_EVENT_H

#include <stack>
#include <utility>
#include <variant>
#include <vector>

#include "event_types.h"
#include "rulesets.h"
#include "target.h"
#include "types.h"


// an event holds a vector of subscribers
// when it fires, each is called

// our Listener will derive from EventListener<Listener>
// which holds a list of events it is subscribed to.
// As these events will have different sigs, we need a base-class.
// We will store pointers to this base-class.
template < typename Subscriber >
class EventBase {
  public:
   virtual void unsubscribe(Subscriber* t) = 0;
};

template < class Subscriber, class Context, events::EventType e_type, class... Args >
class Event: public EventBase< Subscriber > {
  public:
   using signature = std::tuple<Args...>;

  private:
   std::vector< Subscriber* > subscribers;

  protected:
   void _notify(Subscriber* subscriber, Context& context, Args... args)
   {
      (*subscriber)(context, std::make_tuple(std::forward< Args >(args)...));
   }

  public:
   constexpr static events::EventType event_type() { return e_type; };
   const auto& get_subscribers() const { return subscribers; }

   virtual void trigger(Context& context, Args... args)
   {
      for(auto& subscriber : subscribers) {
         _notify(subscriber, context, std::forward< Args >(args)...);
      }
   }
   void subscribe(Subscriber* t) { subscribers.push_back(t); }

   void unsubscribe(Subscriber* t) final
   {
      auto to_remove = std::remove(subscribers.begin(), subscribers.end(), t);
      subscribers.erase(to_remove, subscribers.end());
   }
};


#endif  // LORAINE_EVENT_H
