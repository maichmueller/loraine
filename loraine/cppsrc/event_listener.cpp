
#include "event/event_listener.h"

namespace events {

void EventListener::on_event(Game& game, Player player, const events::AnyEvent& event)
{
   for(auto& listeners :
       std::array{access_listeners(player), access_listeners(Player(1 - player))}) {
      auto& cards_with_effect = listeners[size_t(event.get_event_type())];
      auto opt_causing_cards = event.get_causing_cards();

      auto trigger_card_lambda = [&](const sptr< Card >& card) {
         bool all_consumed = (*card)(game, event);
         if(all_consumed) {
            cards_with_effect.erase(card);
         }
      };

      if(opt_causing_cards.has_value()) {
         // causing cards get preferential treatment in terms of execution order of the event
         // trigger. This allows the order to be correct for e.g. Play-effects of the played card
         // first
         auto causing_cards = opt_causing_cards.value();
         while(not causing_cards.empty()) {
            auto causing_card = causing_cards.back();
            causing_cards.pop_back();

            auto end_iter = cards_with_effect.end();
            auto search_iter = std::find(cards_with_effect.begin(), end_iter, causing_card);
            if(search_iter != end_iter) {
               trigger_card_lambda(*search_iter);
            }
         }
      }
      for(auto card_iter = cards_with_effect.begin(); card_iter != cards_with_effect.end();
          ++card_iter) {
         trigger_card_lambda(*card_iter);
      }
   }
}
void EventListener::unregister_card(const sptr< Card >& card)
{
   for(auto& listener : access_listeners(card->get_owner())) {
      auto end = listener.end();
      auto pos_it = std::find(listener.begin(), end, card);
      if(pos_it != end) {
         listener.erase(pos_it);
      }
   }
}

}  // namespace events