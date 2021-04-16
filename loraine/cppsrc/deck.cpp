
#include "loraine/core/deck.h"

#include <algorithm>
#include <sstream>

#include "loraine/cards/card.h"
#include "loraine/utils/random.h"

entt::entity Deck::pop_by_index(size_t index)
{
   if(auto deck_size = m_cards.size(); index >= deck_size) {
      throw std::out_of_range(
         "Index " + std::to_string(index) + " out of bounds for deck of size "
         + std::to_string(deck_size) + ".");
   }
   auto rbegin = m_cards.rbegin();
   auto pos = std::next(rbegin, index);
   auto popped_card = *pos;
   m_cards.erase(pos.base());
   return popped_card;
}

Deck::Deck(const Deck& other) : m_cards(), m_regions(other.m_regions)
{
   m_cards.reserve(other.m_cards.size());
   for(const auto& card : other.m_cards) {
      m_cards.emplace_back(card->clone());
   }
}

