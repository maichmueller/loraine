
#ifndef LORAINE_EVENT_H
#define LORAINE_EVENT_H

#include <queue>
#include <set>
#include <stack>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

#include "engine/gamedefs.h"
#include "event_types.h"
#include "target.h"
#include "utils/types.h"
#include "utils/utils.h"

class State;

/**
 * The EventCallInterface is a helper struct designed to bring the specific event_call overload
 * to any inheriting class that listens to the specific events. This allows to export a
 * minimal interface.
 * @tparam FirstEvent
 * @tparam RestEvents
 */
template < typename FirstEvent, typename... RestEvents >
struct EventCallInterface: public EventCallInterface< RestEvents... > {
   /**
    * The call to trigger the effect's changes on the status. Calls internally the private function
    * `_event_call`.
    * @param state State,
    *   the state of the associated game
    * @param args Parameter-Pack,
    *   the specific arguments of the subscribed to event
    */
   virtual void event_call(State& state, typename FirstEvent::SignatureTuple&& arg_tuple) = 0;
};
/**
 * Single Event deduction end. Any combination of ListenerTypes, such as Listener<AttackEvent,
 * DamageEvent>, will inherit from each individual call pattern and thus be eligible as subscriber
 * of the individual events.
 * @tparam Event
 */
template < typename Event >
struct EventCallInterface< Event > {
   virtual void event_call(State& state, typename Event::SignatureTuple&& arg_tuple) = 0;
};

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


template < class EventT, class... Args >
class Event: public EventBase {
  public:
   using SignatureTuple = std::tuple< EventT&, Args... >;
   // classes inheriting from effectcallhelper are the actual listeners,
   // but we only need the crude interface
   using Subscriber = EventCallInterface< Event< EventT, Args... > >;

  private:
   std::vector< Subscriber* > subs;
   EventT event_ref;

  protected:
   void _notify(Subscriber* subscriber, State& state, Args... args)
   {
      // passing in the event_ref is needed to distinguish among the event_call overloads
      subscriber->event_call(state, SignatureTuple(event_ref, args...));
   }

  public:
   constexpr static auto event_type() {return EventT::value;}
   const auto& subscribers() const { return subs; }

   virtual void trigger(State& state, Args... args)
   {
      for(auto& sub : subs) {
         _notify(sub, state, args...);
      }
   }
   void subscribe(Subscriber* t) { subs.push({t, 1}); }

   void unsubscribe(void* t) final { subs.erase(static_cast< Subscriber* >(t)); }
};

#endif  // LORAINE_EVENT_H
