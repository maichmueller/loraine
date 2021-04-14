
#ifndef LORAINE_EVENTBUS_H
#define LORAINE_EVENTBUS_H

#include "helpers.h"
#include "event.h"

namespace events {

struct IAllEventSubscriber:
    public IEventSubscriber< std::decay_t< decltype(helpers::make_eventsub_interface(
       std::declval< std::make_index_sequence< n_events > >())) > > {
};

using AllSubChannels = std::decay_t< decltype(helpers::make_subchannel(
   std::declval< std::make_index_sequence< n_events > >())) >;

class EventBus {
  private:
   AllSubChannels m_sub_channels;

  public:
   template < EventLabel label >
   inline const auto& subscribers() const
   {
      return std::get< static_cast< size_t >(label) >(m_sub_channels);
   }

   template < EventLabel label, typename GameStateType, typename... Args >
   void anounce_event(GameStateType& state, Args... args)
   {
      using EventType = helpers::label_to_event_t< label >;
      auto event = EventType{args...};
      event.trigger(state);
      inform(state, event);
   }

   template < EventLabel label >
   inline void subscribe(IEventSubscriber< helpers::label_to_event_t< label > >* t)
   {
      subscribers< label >().emplace_back(t);
   }

   template < EventLabel label >
   inline void unsubscribe(IEventSubscriber< helpers::label_to_event_t< label > >* sub)
   {
      auto& subs = subscribers< label >();
      subs.erase(std::find(subs.begin(), subs.end(), sub));
   }

   template < typename Event, typename GameStateType>
   inline void inform(GameStateType& state, Event& event)
   {
      for(auto& sub : subscribers< Event::label >()) {
         sub->on_event(state, event);
      }
   }
};

}  // namespace events

#endif  // LORAINE_EVENTBUS_H
