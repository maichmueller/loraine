
#include "event_listener.h"

void EventListener::register_card(const sptr< Card >& card)
{
   for(auto& [eve_type, effect] : card->get_effects()) {
      if(! effect.is_null()) {
         listeners[eve_type][card->get_uuid()] = card;
      }
   }
}


void EventListener::on_event(State & state, const events::VariantEvent & e)
{
   events::EventType eve_type = events::get_event_type(e);
   auto cards_with_effect = listeners[eve_type];
   if(cards_with_effect.size() > 1) {
      // shuffle the cards to not have the insertion order decide the execution
      // order
      std::shuffle(
         cards_with_effect.begin(),
         cards_with_effect.end(),
         sadfgsadasfdgfsd;
   }
   for(auto& card_sptr : cards_with_effect) {
      (*card_sptr.second)(state, e, eve_type);
   }
}
void EventListener::unregister_card(const sptr< Card >& card) {
   for (auto & listener : listeners) {
      auto & card_map = listener.second;
      card_map.erase(card->get_uuid());
   }
}
