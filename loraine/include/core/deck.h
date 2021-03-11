
#ifndef LORAINE_DECK_H
#define LORAINE_DECK_H

#include <random.h>

#include <functional>
#include <map>
#include <set>
#include <utility>
#include <vector>

#include "cards/card_defs.h"
#include "utils/types.h"

class Card;

class Deck {
  public:
   // vectors of sptrs should be fastest when cache locality is considered
   // (also for insertion operations)
   using ContainerType = std::vector< sptr< Card > >;
   using FilterFunc = std::function< bool(const sptr< Card >&) >;

   using value_type = typename ContainerType::value_type;
   using pointer = typename ContainerType::pointer;
   using reference = typename ContainerType::reference;
   using const_reference = typename ContainerType::const_reference;
   using iterator = typename ContainerType::iterator;
   using const_iterator = typename ContainerType::const_iterator;
   using reverse_iterator = typename ContainerType::reverse_iterator;
   using const_reverse_iterator = typename ContainerType::const_reverse_iterator;
   using size_type = typename ContainerType::size_type;
   using difference_type = typename ContainerType::difference_type;

   Deck() : m_cards(), m_regions() {}
   Deck(std::initializer_list< value_type > cards) : m_cards(cards) {}
   explicit Deck(ContainerType cards) : m_cards(std::move(cards)) {}
   explicit Deck(ContainerType&& cards) : m_cards(std::move(cards)) {}

   /// forwarded functions from container

   [[nodiscard]] auto size() const { return m_cards.size(); }
   [[nodiscard]] auto begin() const { return m_cards.begin(); }
   [[nodiscard]] auto begin() { return m_cards.begin(); }
   [[nodiscard]] auto end() const { return m_cards.end(); }
   [[nodiscard]] auto end() { return m_cards.end(); }
   [[nodiscard]] inline auto& operator[](size_t n) { return m_cards[n]; }
   [[nodiscard]] inline auto& operator[](size_t n) const { return m_cards[n]; }
   [[nodiscard]] inline const auto& at(size_t idx) const { return m_cards.at(idx); }
   [[nodiscard]] inline auto& at(size_t idx) { return m_cards.at(idx); }
   [[nodiscard]] inline auto erase(const_iterator position) { return m_cards.erase(position); }

   /// actual member logic

   /*
    * Method to filter out specific cards
    * as decided by the filter.
    * This CAN pop the filtered cards.
    */
   std::vector< sptr< Card > > find(const FilterFunc& filter, bool pop);

   /**
    * Shuffle a card into the top n cards of the ContainerType.
    * If top_n == 0 (default), the card is shuffled randomly
    * somewhere into the deck.
    * @param card shared_ptr<Card>,
    *   the pointer to the reshuffled card
    * @param top_n size_t,
    *   the range from the top to shuffle the card into
    */
    template <typename RNG>
   void shuffle_into(const sptr< Card >& card, size_t top_n, RNG&& rng);

   /**
    * Draw the top card
    */
   sptr< Card > draw_card();

   /**
    * Draw the card at a specific index
    */
   sptr< Card > draw_by_index(size_t index);

   /**
    * Drawing randomly one of the cards that matches the given ID.
    * @param card_code const char*,
    *   the card code that needs to be matched
    */
    template <class RNG>
   sptr< Card > draw_by_code(const char* card_code, RNG&& rng);

   template < class RNG >
   inline void shuffle(RNG&& g)
   {
      std::shuffle(begin(), end(), std::forward<RNG>(g));
   }

   static std::set< Region > identify_regions(const ContainerType& container);
   static std::set< Region > identify_regions(std::initializer_list< value_type > cards);

  private:
   ContainerType m_cards;
   // all the regions present in the given cards
   std::set< Region > m_regions;

   /**
    * Method to filter the indices of specific cards
    * as decided by the filter.
    * This does NOT pop the filtered indices.
    */
   [[nodiscard]] std::vector< size_t > _find_indices(const FilterFunc& filter) const;

   std::vector< sptr< Card > > _pop_cards(const FilterFunc& filter);

   [[nodiscard]] std::vector< sptr< Card > > _find_cards(const FilterFunc& filter) const;
};

#endif  // LORAINE_DECK_H
