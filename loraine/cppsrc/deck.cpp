
#include "core/deck.h"

#include <algorithm>
#include <sstream>

#include "cards/card.h"
#include "utils/random.h"

std::vector< size_t > Deck::_find_indices(const FilterFunc& filter) const
{
   std::vector< size_t > indices;
   indices.reserve(m_cards.size());
   for(size_t i = 0; i < m_cards.size(); ++i) {
      if(filter(m_cards[i])) {
         indices.emplace_back(i);
      }
   }
   indices.shrink_to_fit();
   return indices;
}

auto Deck::_pop_cards(const FilterFunc& filter) -> std::vector< sptr< Card > >
{
   std::vector< sptr< Card > > popped_cards;
   auto indices = _find_indices(filter);
   // reserve the memory so that emplace_back doesn't trigger repeated memory relocation
   popped_cards.reserve(indices.size());
   // with every pop we need to calculate the new filtered indices as per
   // shift by 1
   size_t idx_shift = 0;

   for(const auto& idx : indices) {
      size_t shifted_idx = idx - idx_shift;
      popped_cards.emplace_back(pop_by_index(shifted_idx));
      ++idx_shift;
   }
   return popped_cards;
}
auto Deck::_find_cards(const FilterFunc& filter) const -> std::vector< sptr< Card > >
{
   std::vector< sptr< Card > > cards;
   auto indices = _find_indices(filter);
   // reserve the memory so that emplace_back doesn't trigger repeated memory relocation
   cards.reserve(indices.size());

   for(const auto& idx : indices) {
      cards.emplace_back(m_cards[idx]);
   }
   return cards;
}

auto Deck::find(const FilterFunc& filter, bool pop) -> std::vector< sptr< Card > >
{
   if(pop) {
      return _pop_cards(filter);
   }
   return _find_cards(filter);
}

sptr< Card > Deck::pop_by_index(size_t index)
{
   if(auto deck_size = m_cards.size(); index >= deck_size) {
      throw std::out_of_range(
         "Index " + std::to_string(index) + " out of bounds for deck of size "
         + std::to_string(deck_size) + ".");
   }
   auto rbegin = m_cards.rbegin();
   auto pos = std::next(rbegin, index);
   sptr< Card > popped_card = *pos;
   m_cards.erase(pos.base());
   return popped_card;
}

std::set< Region > Deck::identify_regions(const Deck::ContainerType& container)
{
   std::set< Region > rs;
   for(const auto& cptr : container) {
      rs.emplace(cptr->immutables().region);
   }
   return rs;
}

std::set< Region > Deck::identify_regions(std::initializer_list< value_type > cards)
{
   return identify_regions(ContainerType(cards));
}

Deck::Deck(const Deck& other) : m_cards(), m_regions(other.m_regions)
{
   m_cards.reserve(other.m_cards.size());
   for(const auto& card : other.m_cards) {
      m_cards.emplace_back(card->clone());
   }
}

