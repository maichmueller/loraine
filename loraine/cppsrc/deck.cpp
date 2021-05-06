
#include "loraine/core/deck.hpp"

#include <algorithm>
#include <sstream>

#include "loraine/utils/random.h"

entt::entity Deck::pop_by_index(size_t index)
{
   if(auto deck_size = size(); index >= deck_size) {
      throw std::out_of_range(
         "Index " + std::to_string(index) + " out of bounds for deck of size "
         + std::to_string(deck_size) + ".");
   }
   auto pos = std::next(rbegin(), index);
   auto popped_card = *pos;
   erase(pos.base());
   return popped_card;
}


