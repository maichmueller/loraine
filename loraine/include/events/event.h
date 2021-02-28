
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

template < typename... Args >
class Effect;
class State;

// an event holds a vector of sub_queue
// when it fires, each is called

// our Listener will derive from EventListener<Listener>
// which holds a list of events it is subscribed to.
// As these events will have different sigs, we need a base-class.
// We will store pointers to this base-class.
class EventBase {
  public:
   virtual void unsubscribe(void* t) = 0;
};


template < typename Subscriber >
using sub_pair = std::pair< Subscriber*, size_t >;
template < typename Subscriber >
using container_type = std::vector< Subscriber >;

// 'greater' lets the prio queue choose the 'smallest' next element
template < typename Subscriber >
using comparator = std::greater< utils::getter_t< 1, sub_pair< Subscriber > > >;

template < typename Subscriber >
class SubscriberQueue:
    public std::priority_queue<
       sub_pair< Subscriber >,
       container_type< Subscriber >,
       comparator< Subscriber > > {
   static_assert(Subscriber::uuid);

  public:
   using base = std::priority_queue<
      sub_pair< Subscriber >,
      std::vector< sub_pair< Subscriber > >,
      comparator< Subscriber > >;
   using value_type = typename base::value_type;
   using container_type = typename base::container_type;
   /**
    * Find the subscriber in the underlying container and return an iterator to it.
    * @param sub Subscriber*,
    *    the subscriber to search
    * @return container_type::iterator,
    *    the iterator pointing to the found element or the container's end position if not.
    */
   typename container_type::iterator find(Subscriber* sub)
   {
      return std::find_if(base::c.begin(), base::c.end(), [&](const value_type& entry) {
         return _extract_sub(entry) == sub;
      });
   }
   /**
    * Find the uuid of the subscriber and return an iterator to it.
    * @param sub Subscriber*,
    *    the subscriber to search
    * @return std::unordered_set<UUID>::iterator,
    *    the iterator pointing to the UUID of the found element or the container's end position if
    *    not.
    */
   typename std::unordered_set< UUID >::iterator find_id(Subscriber* sub)
   {
      return std::find(m_seen.begin(), m_seen.end(), sub->uuid());
   }
   inline void erase(typename container_type::iterator pos) { base::c.erase(pos); }
   inline void erase(Subscriber* sub)
   {
      erase(find(sub));
      m_seen.erase(sub->uuid());
   }
   /**
    * @brief Updates a subscriber's priority
    *
    * if the subscriber is currently subscribed, then its priority is reset to the given value.
    * Otherwise, nothing is done.
    */
   inline void update(Subscriber* sub, size_t priority)
   {
      if(find_id(sub) == m_seen.end()) {
         // the sub is NOT subscribed
         return;
      }
      // erase the old priority entry
      base::c.erase(find(sub));
      // emplace the sub anew with the new priority
      emplace(sub, priority);
   }

   void push(const value_type& value)
   {
      base::push_back(value);
      _emplace_id(value);
   }

   void push(value_type&& value)
   {
      _emplace_id(value);
      base::push_back(std::move(value));
   }

   auto emplace(value_type&& val)
   {
      _emplace_id(val);
      base::emplace(std::move(val));
   }
   void pop()
   {
      auto to_pop_elem = base::top();
      _erase_id(to_pop_elem);
      base::pop();
   }

  private:
   std::unordered_set< UUID > m_seen;

   auto _extract_sub(const value_type& value) { return std::get< 0 >(value); }
   void _emplace_id(const Subscriber* sub) { m_seen.emplace(sub->uuid()); }
   void _erase_id(const Subscriber& sub) { m_seen.erase(sub->uuid()); }
   void _emplace_id(const value_type& value) { _emplace_id(_extract_sub(value)); }
   void _erase_id(const value_type& value) { _erase_id(_extract_sub(value)); }
};


template < class Derived, class EventT, class... Args >
class Event: public EventBase, public utils::CRTP< Event, Derived, Args... > {
  public:
   using SignatureTuple = std::tuple< Args... >;
   using Subscriber = Effect< Args... >;

  private:
   SubscriberQueue<Subscriber> sub_queue;

  protected:
   void _notify(Subscriber* subscriber, State& state, Args... args)
   {
      subscriber->event_call(state, args...);
   }

  public:
   constexpr static EventT event_type() { return EventT::value; };
   const auto& subscribers() const { return sub_queue; }

   virtual void trigger(State& state, Args... args)
   {
      for(auto& sub : sub_queue) {
         _notify(sub, state, args...);
      }
   }
   void subscribe(Subscriber* t) { sub_queue.push_back(t); }

   void unsubscribe(void* t) final
   {
      sub_queue.erase(static_cast< Subscriber* >(t));
   }

};

#endif  // LORAINE_EVENT_H
