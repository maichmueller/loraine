
#include "event_listener.h"

void EventListener::trigger(State & state, const events::VariantEvent& e)
{
   events::EventType eve_type = std::visit(events::VisitorEventType{}, e);
   auto kk = std::visit(events::VisitorPlayerID{}, e);
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
void EventListener::register_card(const sptr< Card >& card)
{
   for(auto& [eve_type, effect] : card->get_effects()) {
      if(! effect.is_null()) {
         listeners[eve_type].emplace_back(card);
      }
   }
}