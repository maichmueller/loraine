//
// Created by michael on 30.05.20.
//

#ifndef LORAINE_DECK_H
#define LORAINE_DECK_H

#include <functional>
#include <map>
#include <vector>

#include "cards/card.h"
#include "types.h"

/*
 * Deck requirements:
 * - Pop efficiently at end
 * - draw any category of cards individually (spell, champion, follower,...)
 * -
 */

class Deck {
   // vectors of sptrs should be fastest when cache locality is considered
   // (also for insertion operations)
   std::vector< sptr< Card > > m_cards;

   /*
    * Conversion to spell card ptr.
    */
   static inline sptr< Spell > _to_spell(const sptr< Card >& card);

   /*
    * Conversion to unit card ptr.
    */
   static inline sptr< Unit > _to_unit(const sptr< Card >& card);

   /*
    * Method to filter the indices of specific cards
    * as decided by the filter.
    * This does NOT pop the filtered indices.
    */
   std::vector< u32 > _find_indices(
      const std::function< bool(sptr< Card >) >& filter) const;

   std::vector< sptr< Card > > _find_cards_pop(
      const std::function< bool(sptr< Card >) >& filter);

   std::vector< sptr< Card > > _find_cards_nopop(
      const std::function< bool(sptr< Card >) >& filter) const;

  public:
   /*
    * Method to filter out specific spells
    * as decided by the filter.
    * This does pop the filtered cards.
    */
   std::vector< sptr< Card > > find_spells(
      const std::function< bool(sptr< Card >) >& filter);

   /*
    * Method to filter out specific units
    * as decided by the filter
    * This does pop the filtered cards.
    */
   std::vector< sptr< Card > > find_units(
      const std::function< bool(sptr< Card >) >& filter);

   /*
    * Method to filter out specific spells
    * as decided by the filter.
    * This CAN pop the filtered cards.
    */
   std::vector< sptr< Card > > find_cards(
      const std::function< bool(sptr< Card >) >& filter, bool pop);

   /*
    * Shuffle a card into the top n cards of the deck.
    * If top_n == 0 (default), the card is shuffled randomly
    * into the deck without restriction on depth.
    */
   void shuffle_into_deck(const sptr< Card >& card, size_t top_n = 0);

   /*
    * Draw the top card
    */
   sptr< Card > draw_card();
   /*
    * Drawing randomly on of the cards that matches the given ID.
    */
   sptr< Card > draw_specific_card(SID card_sid);
};

#endif  // LORAINE_DECK_H
