
#ifndef LORAINE_EVENTBASE_H
#define LORAINE_EVENTBASE_H


// forward-declare
class State;

// an event holds a vector of subs
// when it fires, each is called

// our Listener will derive from EventListener<Listener>
// which holds a list of events it is subscribed to.
// As these events will have different signatures, we need a base-class.
// We will store pointers to this base-class.
class EventBase {
  public:
   virtual void unsubscribe(void* t) = 0;
};

/**
 * The EventCallInterface is a helper struct designed to bring the specific event_call overload
 * to any inheriting class that listens to the specific events. This allows to export a
 * minimal interface.
 * @tparam FirstEvent
 * @tparam RestEvents
 */
template < typename FirstEvent, typename... RestEvents >
struct EventCallInterface:
   public EventCallInterface< FirstEvent >, /* to count as subscriber to FirstEvent and have the
                                              respective event_call overload */
   public EventCallInterface< RestEvents... > /* to become eligible as subscriber to the remaining
                                                  events listed in the variadic pack with respective
                                                  event_call method*/
{
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

#endif  // LORAINE_EVENTBASE_H
