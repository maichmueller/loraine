//
// Created by michael on 30.05.20.
//

#include "deck.h"

#include <algorithm>
#include <sstream>

#include "rng_machine.h"

std::vector< size_t > CardContainer::_find_indices(const FilterFunc& filter) const
{
   std::vector< size_t > indices;
   for(auto i = 0ul; i < m_cards.size(); ++i) {
      if(filter(m_cards[i])) {
         indices.push_back(i);
      }
   }
   return indices;
}

std::vector< sptr< Card > > CardContainer::find_spells(const FilterFunc& filter)
{
   return _pop_cards([&](const sptr< Card >& card) {
      return card->get_const_attrs().card_type == CardType::SPELL && filter(card);
   });
}

std::vector< sptr< Card > > CardContainer::find_units(const FilterFunc& filter)
{
   return _pop_cards([&](const sptr< Card >& card) {
      return card->get_const_attrs().card_type == CardType::UNIT && filter(card);
   });
}
sptr< Card > CardContainer::draw_specific_card(const char* card_sid)
{
   auto indices = _find_indices(
      [&card_sid](const sptr< Card >& card) { return card->get_const_attrs().code == card_sid; });
   std::shuffle(indices.begin(), indices.end(), rng::get_engine());
   return draw_card_by_index(indices[0]);
}

auto CardContainer::_pop_cards(const FilterFunc& filter) -> std::vector< sptr< Card > >
{
   std::vector< sptr< Card > > popped_cards;
   auto indices = _find_indices(filter);

   // with every pop we need to calculate the new filtered indices as per
   // shift by 1
   size_t idx_shift = 0;

   for(const auto& idx : indices) {
      size_t shifted_idx = idx - idx_shift;
      popped_cards.emplace_back(draw_card_by_index(shifted_idx));
      ++idx_shift;
   }
   return popped_cards;
}
auto CardContainer::_find_cards(const FilterFunc& filter) const -> std::vector< sptr< Card > >
{
   std::vector< sptr< Card > > cards;
   auto indices = _find_indices(filter);

   for(const auto& idx : indices) {
      cards.emplace_back(m_cards[idx]);
   }
   return cards;
}

auto CardContainer::find_cards(const FilterFunc& filter, bool pop) -> std::vector< sptr< Card > >
{
   if(pop) {
      return _pop_cards(filter);
   } else {
      return _find_cards(filter);
   }
}

void CardContainer::shuffle_into(const sptr< Card >& card, size_t top_n)
{
   auto deck_size = m_cards.size();
   if(top_n > deck_size) {
      top_n = deck_size;
   }
   auto max = std::min(top_n, deck_size);
   std::uniform_int_distribution< size_t > dist(0, max);
   auto pos = m_cards.begin() + (deck_size - dist(rng::get_engine()));
   m_cards.insert(pos, card);
}

sptr< Card > CardContainer::draw_card()
{
   sptr< Card > card = m_cards.back();
   m_cards.pop_back();
   return card;
}
sptr< Card > CardContainer::draw_card_by_index(size_t index)
{
   if(auto deck_size = m_cards.size(); index + 1 > deck_size) {
      std::stringstream msg;
      msg << "Index " << std::to_string(index) << " out of bounds for deck of size "
          << std::to_string(deck_size) << ".";
      throw std::logic_error(msg.str());
   }
   auto begin = std::begin(m_cards);
   auto pos = begin - index;
   sptr< Card > popped_card = m_cards.at(std::distance(pos, begin) - 1);
   m_cards.erase(pos);
   return popped_card;
}