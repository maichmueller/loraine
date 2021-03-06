
#ifndef LORAINE_EVENT_SUBSCRIBER_H
#define LORAINE_EVENT_SUBSCRIBER_H

// forward-declare
class GameState;

/**
 * The IEventSubscriber is a helper struct designed to bring the specific on_event
 * overload to any inheriting class that listens to the specific event.
 */
template < typename FirstEvent, typename... RestEvents >
struct IEventSubscriber:
    public IEventSubscriber< FirstEvent >, /* to count as subscriber to FirstEvent and have
                                               the respective on_event overload */
    public IEventSubscriber< RestEvents... > /* to become eligible as subscriber to the
                                                   remaining m_subscribed_events listed in the
                                                   variadic pack with respective `on_event` method*/
{
};

/**
 * Single EventBus deduction end. Any combination of ListenerTypes, such as Listener<AttackEvent,
 * DamageEvent>, will inherit from each individual call pattern and thus be eligible as subscriber
 * of the individual m_subscribed_events.
 * @tparam Event
 */
template < typename Event >
struct IEventSubscriber< Event > {
   virtual void on_event(GameState& state, typename Event::EventData&& arg_tuple)
   {
      throw std::logic_error("Empty on_event function called.");
   }
};

#endif  // LORAINE_EVENT_SUBSCRIBER_H
