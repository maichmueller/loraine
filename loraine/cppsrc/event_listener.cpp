
#include "event/event_listener.h"

#include "rng_machine.h"

namespace events {

EventListener event_listener;

void EventListener::register_card(const sptr< Card >& card)
{
   for(const auto& [eve_type, effect] : card->get_effects_map()) {
      if(! effect.is_null()) {
         (*this)[eve_type].emplace_back(card);
      }
   }
}

void EventListener::on_event(const events::VariantEvent& e)
{
   if(! _state_is_set()) {
      return;
   }

   const events::EventType eve_type = events::get_event_type(e);
   auto cards_with_effect = (*this)[eve_type];
   size_t n_effects = cards_with_effect.size();

   if(n_effects > 1) {
      // shuffle to guarantee randomness in the execution order of the effects,
      // as there is no rule for execution order (as of yet).
      std::shuffle(
         cards_with_effect.begin(),
         cards_with_effect.end(),
         rng::rng_def_engine);
   }
   for(auto& card_sptr : cards_with_effect) {
      (*card_sptr)(*m_game_ref, e);
   }
}
void EventListener::unregister_card(const sptr< Card >& card)
{
   for(auto& listener : m_listeners) {
      auto end = listener.end();
      auto pos_it = std::find(listener.begin(), end, card);
      if(pos_it != end)
         listener.erase(pos_it);
   }
}
void EventListener::register_game(Game& game, bool clear)
{
   m_game_ref = &game;
   if(clear) {
      clear_registers();
   }
}
void EventListener::unregister_state()
{
   m_game_ref = nullptr;
   clear_registers();
}
void EventListener::clear_registers()
{
   for(auto& listener : m_listeners) {
      listener = ListenerType();
   }
}

}  // namespace events