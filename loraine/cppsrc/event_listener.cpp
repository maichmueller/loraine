
#include "events/event_listener.h"

namespace events {

void EventListener::on_event(State& state, const events::AnyEvent& event)
{
   Player player = event.get_player();
   for(auto& listeners :
       std::array{access_listeners(player), access_listeners(Player(1 - player))}) {
      auto& registered_cards = listeners.at(size_t(event.get_event_type()));
      auto opt_causing_cards = event.get_causing_cards();

      auto trigger_card_lambda = [&](const sptr< Card >& card) {
         bool all_consumed = (*card)(state, event);
         if(all_consumed) {
            registered_cards.erase(card);
         }
      };

      if(opt_causing_cards.has_value()) {
         // causing cards get preferential treatment in terms of execution order of the events
         // trigger. This allows the order to be correct for e.g. Play-effects of the played card
         // first
         auto causing_cards = opt_causing_cards.value();
         while(not causing_cards.empty()) {
            auto causing_card = causing_cards.back();
            causing_cards.pop_back();

            auto end_iter = registered_cards.end();
            auto search_iter = std::find(registered_cards.begin(), end_iter, causing_card);
            if(search_iter != end_iter) {
               trigger_card_lambda(*search_iter);
            }
         }
      }
      // trigger now every remaining card that is listening to this events
      std::for_each(registered_cards.begin(), registered_cards.end(), trigger_card_lambda);
   }
}
void EventListener::unregister_card(const sptr< Card >& card)
{
   for(auto& listener : access_listeners(card->get_mutable_attrs().owner)) {
      auto end = listener.end();
      auto pos_it = std::find(listener.begin(), end, card);
      if(pos_it != end) {
         listener.erase(pos_it);
      }
   }
}

}  // namespace events