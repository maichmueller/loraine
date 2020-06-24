//
// Created by michael on 30.05.20.
//

#include "deck.h"

#include <algorithm>

#include "rng_machine.h"

std::vector< u32 > Deck::_find_indices(
   const std::function< bool(sptr< Card >) >& filter) const
{
   std::vector< u32 > indices;
   for(auto i = 0; i < m_cards.size(); ++i) {
      if(filter(m_cards[i])) {
         indices.push_back(i);
      }
   }
   return indices;
}

std::vector< sptr< Card > > Deck::find_spells(
   const std::function< bool(sptr< Card >) >& filter)
{
   return _find_cards_pop([&](sptr< Card > card) {
      if(card->get_card_type() == CardType::SPELL && filter(card)) {
         return true;
      }
   });
}

std::vector< sptr< Card > > Deck::find_units(
   const std::function< bool(sptr< Card >) >& filter)
{
   return _find_cards_pop([&](sptr< Card > card) {
      if(card->get_card_type() == CardType::UNIT && filter(card)) {
         return true;
      }
   });
}
sptr< Card > Deck::draw_specific_card(SID card_sid)
{
   return m_cards[_find_indices([](sptr< Card > card) { return true; })[0]];
}

sptr< Spell > Deck::_to_spell(const sptr< Card >& card)
{
   return std::dynamic_pointer_cast< Spell >(card);
}
sptr< Unit > Deck::_to_unit(const sptr< Card >& card)
{
   return std::dynamic_pointer_cast< Unit >(card);
}
auto Deck::_find_cards_pop(const std::function< bool(sptr< Card >) >& filter)
   -> std::vector< sptr< Card > >
{
   std::vector< sptr< Card > > cards;
   auto indices = _find_indices(filter);

   // with every pop we need to calculate the new filtered indices as per
   // shift by 1
   size_t idx_shift = 0;

   for(const auto& idx : indices) {
      size_t shifted_idx = idx - idx_shift;
      cards.emplace_back(m_cards[shifted_idx]);
      m_cards.erase(m_cards.begin() + shifted_idx);
      ++idx_shift;
   }
   return cards;
}
auto Deck::_find_cards_nopop(const std::function< bool(sptr< Card >) >& filter)
   const -> std::vector< sptr< Card > >
{
   std::vector< sptr< Card > > cards;
   auto indices = _find_indices(filter);

   for(const auto& idx : indices) {
      cards.emplace_back(m_cards[idx]);
   }
   return cards;
}

auto Deck::find_cards(
   const std::function< bool(sptr< Card >) >& filter, bool pop)
   -> std::vector< sptr< Card > >
{
   if(pop) {
      return _find_cards_pop(filter);
   } else {
      return _find_cards_nopop(filter);
   }
}

void Deck::shuffle_into_deck(const sptr< Card >& card, size_t top_n)
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
