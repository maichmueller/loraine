
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
#include "event_labels.h"
#include "target.h"
#include "utils/types.h"
#include "utils/utils.h"
#include "eventbase.h"

class State;

template < class EventT, class... Args >
class Event: public EventBase {
  public:
   using SignatureTuple = std::tuple< EventT&, Args... >;
   // classes inheriting from EventCallInterface are the actual subscribers,
   // but we only need the crude interface
   using SelfType = Event< EventT, Args... >;
   using Subscriber = EventCallInterface< SelfType >;

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
   constexpr static auto event_type() { return EventT::value; }
   const auto& subscribers() const { return subs; }

   virtual void trigger(State& state, Args... args);
   void subscribe(Subscriber* t) { subs.push({t, 1}); }

   void unsubscribe(void* t) final { subs.erase(static_cast< Subscriber* >(t)); }
};

#include "engine/state.h"

template < class EventT, class... Args >
void Event< EventT, Args... >::trigger(State& state, Args... args)
{
   for(auto& sub : state.logic()->sort_event_execution< SelfType > subs) {
      _notify(sub, state, args...);
   }
}

#endif  // LORAINE_EVENT_H
