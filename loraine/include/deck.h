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

class DeckContainer {
  public:
   [[nodiscard]] inline auto& get_cards() { return m_cards; }

   [[nodiscard]] inline const auto& get_cards() const { return m_cards; }
   void set_cards(std::vector< sptr< Card > > cards) { m_cards = std::move(cards); }

   /*
    * Method to filter out specific spells
    * as decided by the filter.
    * This does pop the filtered cards.
    */
   std::vector< sptr< Card > > find_spells(const std::function< bool(sptr< Card >) >& filter);

   /*
    * Method to filter out specific units
    * as decided by the filter
    * This does pop the filtered cards.
    */
   std::vector< sptr< Card > > find_units(const std::function< bool(sptr< Card >) >& filter);

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
    * Draw the card at a specific index
    */
   sptr< Card > draw_card_by_index(size_t index);

   /*
    * Drawing randomly one of the cards that matches the given ID.
    */
   sptr< Card > draw_specific_card(const char* card_sid);

   [[nodiscard]] auto size() const { return m_cards.size(); }
   [[nodiscard]] auto begin() const { return m_cards.begin(); }
   [[nodiscard]] auto begin() { return m_cards.begin(); }
   [[nodiscard]] auto end() const { return m_cards.end(); }
   [[nodiscard]] auto end() { return m_cards.end(); }
   [[nodiscard]] inline auto& operator[](size_t n) { return m_cards[n]; }
   [[nodiscard]] inline auto& operator[](size_t n) const { return m_cards[n]; }
   [[nodiscard]] inline const auto& at(size_t idx) const { return m_cards.at(idx); }
   [[nodiscard]] inline auto& at(size_t idx) { return m_cards.at(idx); }

  private:
   // vectors of sptrs should be fastest when cache locality is considered
   // (also for insertion operations)
   std::vector< sptr< Card > > m_cards;

   /*
    * Method to filter the indices of specific cards
    * as decided by the filter.
    * This does NOT pop the filtered indices.
    */
   std::vector< size_t > _find_indices(const std::function< bool(sptr< Card >) >& filter) const;

   std::vector< sptr< Card > > _find_cards_pop(const std::function< bool(sptr< Card >) >& filter);

   std::vector< sptr< Card > > _find_cards_nopop(
      const std::function< bool(sptr< Card >) >& filter) const;
};

#endif  // LORAINE_DECK_H
