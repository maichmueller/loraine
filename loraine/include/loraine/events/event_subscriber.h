
#ifndef LORAINE_EVENT_SUBSCRIBER_H
#define LORAINE_EVENT_SUBSCRIBER_H


namespace events {

/**
 * The IEventSubscriber is a helper struct designed to bring the specific on_event
 * overload to any inheriting class that listens to the specific event.
 */
template < typename Derived, typename FirstEvent, typename... RestEvents >
struct IGameEventListener:
    public IGameEventListener< Derived, FirstEvent >, /* to count as subscriber to FirstEvent and
                                               have the respective on_event overload */
    public IGameEventListener< Derived, RestEvents... > /* to become eligible as subscriber to the
                                                   remaining m_subscribed_events listed in the
                                                   variadic pack with respective `on_event` method*/
{
};

/**
 * Single Event deduction end. Any combination of ListenerTypes, such as Listener<AttackEvent,
 * DamageEvent>, will inherit from each individual call pattern and thus be eligible as subscriber
 * of the individual events.
 * @tparam Event
 */
template < typename Derived, typename Event >
struct IGameEventListener< Derived, Event >:
    public utils::CRTP< IGameEventListener, Derived, Event > {
   using event_type = Event;

   void on_event(const event_type& event) { this->derived()->on_event(event); }
};

///**
// * @brief Provides an unbound method pointer to the potentially overloaded on_event function.
// *
// * The entt::dispatcher class needs a free function or unbound method pointer to connect a listener.
// * Yet all on_event methods are overloaded with the respective event type to listen to. So one needs
// * this helper method this disambiguate the exact on_event overload and get the correct unbound
// * method pointer.
// * @tparam EventT The event type whose on_event overload should be taken.
// * @tparam ConcreteListener The actual listener class that this pointer should take the method from.
// * @return function pointer to the correct unbound on_event method.
// */
//template < typename EventT, class ConcreteListener >
//constexpr auto on_event()
//{
//   return static_cast< void (IGameEventListener< ConcreteListener, EventT >::*)(const EventT&) >(
//      &IGameEventListener< ConcreteListener, EventT >::on_event);
//}

}  // namespace events
#endif  // LORAINE_EVENT_SUBSCRIBER_H
