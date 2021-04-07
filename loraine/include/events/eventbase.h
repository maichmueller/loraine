
#ifndef LORAINE_EVENTBASE_H
#define LORAINE_EVENTBASE_H


// forward-declare
class GameState;

/**
 * The EventSubscriberInterface is a helper struct designed to bring the specific event_call overload
 * to any inheriting class that listens to the specific m_subscribed_events. This allows to export a
 * minimal interface.
 * @tparam FirstEvent
 * @tparam RestEvents
 */
template < typename FirstEvent, typename... RestEvents >
struct EventSubscriberInterface:
   public EventSubscriberInterface< FirstEvent >, /* to count as subscriber to FirstEvent and have the
                                              respective event_call overload */
   public EventSubscriberInterface< RestEvents... > /* to become eligible as subscriber to the remaining
                                                  m_subscribed_events listed in the variadic pack with respective
                                                  event_call method*/
{
};

/**
 * Single Event deduction end. Any combination of ListenerTypes, such as Listener<AttackEvent,
 * DamageEvent>, will inherit from each individual call pattern and thus be eligible as subscriber
 * of the individual m_subscribed_events.
 * @tparam Event
 */
template < typename Event >
struct EventSubscriberInterface< Event > {
   virtual void event_call(GameState& state, typename Event::SignatureTuple&& arg_tuple) = 0;
};

#endif  // LORAINE_EVENTBASE_H
