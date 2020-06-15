//
// Created by michael on 30.05.20.
//

#include "deck.h"

std::vector< sptr< Spell > > Deck::find_spell(
   const std::function< bool(sptr< Card >) >& filter)
{
   std::vector< sptr< Spell > > spells;
   for(auto& card_ptr : m_cards) {
      if(filter(card_ptr)) {
         spells.emplace_back(std::dynamic_pointer_cast<Spell>(card_ptr));
      }
   }

   return spells;
}
std::vector< u32 > Deck::find_indices(
   const std::function< bool(sptr< Card >) >& filter)
{
   std::vector< u32 > indices;
   for(auto i = m_cards.size() - 1; i >= 0; --i) {
      if(filter(m_cards[i])) {
         indices.push_back(i);
      }
   }
   return indices;
}
