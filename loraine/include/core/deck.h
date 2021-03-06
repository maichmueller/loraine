
#ifndef LORAINE_DECK_H
#define LORAINE_DECK_H

#include <utils/random.h>

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

   /// constructors

   Deck() : m_cards(), m_regions() {}
   Deck(std::initializer_list< value_type > cards) : m_cards(cards) {}
   explicit Deck(ContainerType cards) : m_cards(std::move(cards)) {}
   explicit Deck(ContainerType&& cards) : m_cards(std::move(cards)) {}
   Deck(const Deck& other);
   Deck& operator=(const Deck& other) = delete;
   Deck(Deck&& other) = default;
   Deck& operator=(Deck&& other) = default;
   ~Deck() = default;

   /// forwarded functions from container

   [[nodiscard]] auto size() const noexcept { return m_cards.size(); }
   [[nodiscard]] auto begin() const noexcept { return m_cards.begin(); }
   [[nodiscard]] auto begin() noexcept { return m_cards.begin(); }
   [[nodiscard]] auto end() const noexcept { return m_cards.end(); }
   [[nodiscard]] auto end() noexcept { return m_cards.end(); }
   [[nodiscard]] inline auto& operator[](size_t n) { return m_cards[n]; }
   [[nodiscard]] inline auto& operator[](size_t n) const { return m_cards[n]; }
   [[nodiscard]] inline const auto& at(size_t idx) const { return m_cards.at(idx); }
   [[nodiscard]] inline auto& at(size_t idx) { return m_cards.at(idx); }
   [[nodiscard]] inline auto erase(const_iterator position) { return m_cards.erase(position); }
   [[nodiscard]] inline auto empty() const noexcept { return m_cards.empty(); }

   /// actual member logic

   /**
    * Pop the top spell from the stack and return it.
    * @return shared_ptr<Card>,
    *   the popped spell
    */
   inline sptr< Card > pop()
   {
      auto last = m_cards.back();
      m_cards.pop_back();
      return last;
   }

   /*
    * Method to filter out specific cards
    * as decided by the filter.
    * This CAN pop the filtered cards.
    */
   std::vector< sptr< Card > > find(const FilterFunc& filter, bool pop);

   /**
    * Shuffle a spell into the top n cards of the ContainerType.
    * If top_n == 0 (default), the spell is shuffled randomly
    * somewhere into the deck.
    * @param card shared_ptr<Card>,
    *   the pointer to the reshuffled spell
    * @param top_n size_t,
    *   the range from the top to shuffle the spell into
    */
   template < typename RNG >
   void shuffle_into(const sptr< Card >& card, RNG&& rng, size_t top_n);

   /**
    * Draw the spell at a specific index
    */
   sptr< Card > pop_by_index(size_t index);
   /**
    * Draw the cards at specific indices
    */
   template <
      typename Container,
      typename = std::enable_if_t< std::is_integral_v< typename Container::value_type > > >
   std::vector< sptr< Card > > pop_by_index(Container indices);
   template <
      typename IntType,
      typename = std::enable_if_t< std::is_integral_v< IntType > > >
   std::vector< sptr<Card>> pop_by_index(std::initializer_list<IntType> indices);

   /**
    * Drawing randomly one of the cards that matches the given code.
    * @param card_code const char*,
    *   the spell code that needs to be matched
    */
   template < class RNG >
   sptr< Card > pop_by_code(const char* card_code, RNG&& rng);

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


#include "cards/card.h"

template < class RNG >
sptr< Card > Deck::pop_by_code(const char* card_code, RNG&& rng)
{
   auto indices = _find_indices(
      [&card_code](const sptr< Card >& card) { return card->immutables().code == card_code; });
   random::shuffle_inplace(m_cards, rng);
   return pop_by_index(indices[0]);
}

template < typename Container, typename >
std::vector< sptr< Card > > Deck::pop_by_index(Container indices)
{
   std::sort(indices.begin(), indices.end());
   if(not (indices.back() < m_cards.size())) {
      throw std::out_of_range(
         "Indices to pop exceed spell container boundaries: Greatest index = "
         + std::to_string(indices.back()) + " > " + std::to_string(m_cards.size())
         + " = container size.");
   }
   std::vector< sptr< Card > > out;
   out.reserve(indices.size());
   size_t curr_idx = m_cards.size() - 1;
   m_cards.erase(std::remove_if(m_cards.begin(), m_cards.end(), [&](const auto& card) {
      bool match = curr_idx == indices.back();
      curr_idx--;
      if(match) {
         indices.pop_back();
         out.emplace_back(card);
      }
      return match;
   }));
   return out;
}

template <
   typename IntType,
   typename >
std::vector< sptr<Card>> Deck::pop_by_index(std::initializer_list<IntType> indices) {
   return pop_by_index<std::vector<IntType>>(indices);
}

template < typename RNG >
void Deck::shuffle_into(const sptr< Card >& card, RNG&& rng, size_t top_n)
{
   auto deck_size = m_cards.size();
   if(top_n > deck_size) {
      top_n = deck_size;
   }
   auto max = std::min(top_n, deck_size);
   std::uniform_int_distribution< size_t > dist(0, max);
   auto pos = m_cards.begin() + (deck_size - dist(std::forward< RNG >(rng)));
   m_cards.insert(pos, card);
}

#endif  // LORAINE_DECK_H
