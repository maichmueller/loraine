
#ifndef LORAINE_EVENT_LISTENER_H
#define LORAINE_EVENT_LISTENER_H

#include <map>
#include <random>
#include <vector>

#include "cards/card.h"
#include "event.h"
#include "types.h"

// forward-declare state
class State;

class EventListener {
   std::map< events::EventType, std::vector< sptr< Card > > > listeners;
   std::random_device m_rnd_dev;

  public:
   template <typename Event>
   void on_event(State & state, const Event& e);
   void register_card(const sptr< Card >& card);
};


template <typename Event>
void EventListener::on_event(State & state, const Event& e)
{
   events::EventType eve_type = events::get_event_type(e);
   auto cards_with_effect = listeners[eve_type];
   if(cards_with_effect.size() > 1) {
      // shuffle the cards to not have the insertion order decide the execution
      // order
      std::shuffle(
         cards_with_effect.begin(),
         cards_with_effect.end(),
         std::default_random_engine(m_rnd_dev()));
   }
   for(auto& card_sptr : cards_with_effect) {
      (*card_sptr)(state, e, eve_type);
   }
}
#endif  // LORAINE_EVENT_LISTENER_H
