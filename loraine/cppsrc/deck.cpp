//
// Created by michael on 30.05.20.
//

#include "deck.h"

#include <algorithm>
#include <sstream>

#include "rng_machine.h"

std::vector< size_t > DeckContainer::_find_indices(
   const std::function< bool(sptr< Card >) >& filter) const
{
   std::vector< size_t > indices;
   for(auto i = 0ul; i < m_cards.size(); ++i) {
      if(filter(m_cards[i])) {
         indices.push_back(i);
      }
   }
   return indices;
}

std::vector< sptr< Card > > DeckContainer::find_spells(
   const std::function< bool(sptr< Card >) >& filter)
{
   return _find_cards_pop([&](const sptr< Card >& card) {
      return card->get_card_type() == CardType::SPELL && filter(card);
   });
}

std::vector< sptr< Card > > DeckContainer::find_units(
   const std::function< bool(sptr< Card >) >& filter)
{
   return _find_cards_pop([&](const sptr< Card >& card) {
      return card->get_card_type() == CardType::UNIT && filter(card);
   });
}
sptr< Card > DeckContainer::draw_specific_card(SID card_sid)
{
   auto indices = _find_indices([&card_sid](const sptr< Card >& card) {
      return card->get_id() == card_sid;
   });
   std::shuffle(indices.begin(), indices.end(), rng::rng_def_engine);
   return _pop_single_card(indices[0]);
}

sptr< Spell > DeckContainer::_to_spell(const sptr< Card >& card)
{
   return std::dynamic_pointer_cast< Spell >(card);
}
sptr< Unit > DeckContainer::_to_unit(const sptr< Card >& card)
{
   return std::dynamic_pointer_cast< Unit >(card);
}
auto DeckContainer::_find_cards_pop(
   const std::function< bool(sptr< Card >) >& filter)
   -> std::vector< sptr< Card > >
{
   std::vector< sptr< Card > > popped_cards;
   auto indices = _find_indices(filter);

   // with every pop we need to calculate the new filtered indices as per
   // shift by 1
   size_t idx_shift = 0;

   for(const auto& idx : indices) {
      size_t shifted_idx = idx - idx_shift;
      popped_cards.emplace_back(_pop_single_card(shifted_idx));
      ++idx_shift;
   }
   return popped_cards;
}
auto DeckContainer::_find_cards_nopop(
   const std::function< bool(sptr< Card >) >& filter) const
   -> std::vector< sptr< Card > >
{
   std::vector< sptr< Card > > cards;
   auto indices = _find_indices(filter);

   for(const auto& idx : indices) {
      cards.emplace_back(m_cards[idx]);
   }
   return cards;
}

auto DeckContainer::find_cards(
   const std::function< bool(sptr< Card >) >& filter, bool pop)
   -> std::vector< sptr< Card > >
{
   if(pop) {
      return _find_cards_pop(filter);
   } else {
      return _find_cards_nopop(filter);
   }
}

void DeckContainer::shuffle_into_deck(const sptr< Card >& card, size_t top_n)
{
   auto deck_size = m_cards.size();
   if(top_n > deck_size) {
      top_n = deck_size;
   }
   auto max = std::min(top_n, deck_size);
   std::uniform_int_distribution< size_t > dist(0, max);
   auto pos = m_cards.begin() + (deck_size - dist(rng::rng_def_engine));
   m_cards.insert(pos, card);
}

sptr< Card > DeckContainer::draw_card()
{
   sptr< Card > card = m_cards.back();
   m_cards.pop_back();
   return card;
}
sptr< Card > DeckContainer::_pop_single_card(size_t  index)
{
   if(auto deck_size = m_cards.size(); index + 1 > deck_size) {
      std::stringstream msg;
      msg << "Index " << std::to_string(index)
                << " out of bounds for deck of size "
                << std::to_string(deck_size) << ".";
      throw std::logic_error(msg.str());
   }
   auto begin = std::begin(m_cards);
   auto pos = begin - index;
   sptr< Card > popped_card = m_cards.at(std::distance(pos, begin) - 1);
   m_cards.erase(pos);
   return popped_card;
}
