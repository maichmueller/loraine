
#include "event/event_listener.h"

#include "rng_machine.h"

namespace events {

EventListener event_listener;

void EventListener::register_card(const sptr< Card >& card)
{
   for(auto& [eve_type, effect] : card->get_effects_map()) {
      if(! effect.is_null()) {
         (*this)[eve_type][card->get_uuid()] = card;
      }
   }
}

void EventListener::on_event(State& state, const events::VariantEvent& e)
{
   const events::EventType eve_type = events::get_event_type(e);
   auto cards_with_effect = (*this)[eve_type];
   size_t n_effects = cards_with_effect.size();

   if(n_effects > 1) {
      // we copy all effects to be triggered into a vector and the shuffle to
      // guarantee randomness in the execution order of the effects, as there is
      // no rule for execution order.
      std::vector< sptr< Card > > event_effect_cards;
      event_effect_cards.reserve(n_effects);

      for(auto& card_sptr : cards_with_effect) {
         event_effect_cards.emplace_back(card_sptr.second);
      }
      // shuffle the cards to not have the insertion order decide the execution
      // order
      std::shuffle(
         event_effect_cards.begin(),
         event_effect_cards.end(),
         rng::rng_def_engine);

      for(auto& card_sptr : event_effect_cards) {
         (*card_sptr)(state, e, eve_type);
      }
   } else {
      // only one effect is triggered -> no randomness needed
      for(auto& card_sptr : cards_with_effect) {
         (*card_sptr.second)(state, e, eve_type);
      }
   }
}
void EventListener::unregister_card(const sptr< Card >& card)
{
   for(auto& listener : listeners) {
      listener.erase(card->get_uuid());
   }
}

}  // namespace events