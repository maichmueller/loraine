
#ifndef LORAINE_DECK_H
#define LORAINE_DECK_H

#include <entt/entt.hpp>
#include <set>
#include <vector>

#include "loraine/utils/random.h"
#include "loraine/utils/utils.h"
#include "loraine/cards/carddefs.h"

/**
 * A deck is essentially a std vector with a few utility methods added. Maybe a later rewrite to
 * free functions will forego the sin of inheriting from vector
 */
class Deck final: std::vector< entt::entity > {
  public:
   using base = std::vector< entt::entity >;

   /// constructors

   Deck(std::initializer_list< value_type > cards, std::set< Region > regions)
       : base(cards), m_regions(regions)
   {
   }
   template < typename Container >
   Deck(const Container& cards, std::set< Region > regions)
       : base{cards.begin(), cards.end()}, m_regions(regions)
   {
   }
   Deck(const Deck& other) = default;
   Deck& operator=(const Deck& other) = delete;
   Deck(Deck&& other) = default;
   Deck& operator=(Deck&& other) = default;
   ~Deck() = default;

   /// logic

   /**
    * Pop the top spell from the stack and return it.
    * @return shared_ptr<Card>,
    *   the popped spell
    */
   inline entt::entity pop()
   {
      auto last = back();
      pop_back();
      return last;
   }

   /*
    * Method to filter out specific cards
    * as decided by the filter.
    * This CAN pop the filtered cards.
    */
   template < typename Predicate >
   std::vector< entt::entity > find(Predicate f) const;

   /**
    * Shuffle a card into the top n cards of the container.
    * If top_n == 0, the card is shuffled randomly
    * somewhere into the deck.
    * @param card entity,
    *   the card to shuffle
    * @param top_n size_t,
    *   the range from the top to shuffle the card into
    */
   template < typename RNG >
   void shuffle_into(entt::entity card, size_t top_n, RNG&& rng);

   /**
    * Draw the spell at a specific index
    */
   entt::entity pop_by_index(size_t index);
   /**
    * Draw the cards at specific indices
    */
   template <
      typename Container,
      typename = std::enable_if_t< std::is_integral_v< typename Container::value_type > > >
   std::vector< entt::entity > pop_by_index(Container indices);
   template < typename IntType, typename = std::enable_if_t< std::is_integral_v< IntType > > >
   std::vector< entt::entity > pop_by_index(std::initializer_list< IntType > indices);

   /**
    * Drawing the at most n cards that fulfill the predicate.
    */
   template < class Predicate, class RNG >
   entt::entity pop_if(Predicate f, size_t at_most, RNG& rng);

   template < typename RNG >
   void shuffle(RNG& rng);

  private:
   // all the regions present in the given cards
   std::set< Region > m_regions;

   /**
    * Method to filter the indices of specific cards
    * as decided by the filter.
    * This does NOT pop the filtered indices.
    */
   template < class Predicate >
   [[nodiscard]] std::vector< size_t > _find_indices(const Predicate& filter, size_t at_most = 0)
      const;

   template < class Predicate >
   [[nodiscard]] std::vector< entt::entity > _find_cards(Predicate f) const;
};

template < class Predicate >
std::vector< size_t > Deck::_find_indices(const Predicate& filter, size_t at_most) const
{
   std::vector< size_t > indices;
   indices.reserve(std::min(size(), at_most));
   for(size_t i = 0; i < size(); ++i) {
      if(at_most == 0) {
         break;
      }
      if(filter(self[i])) {
         indices.emplace_back(i);
         at_most--;
      }
   }
   indices.shrink_to_fit();
   return indices;
}

template < typename RNG >
void Deck::shuffle(RNG& rng)
{
   random::shuffle_inplace(self, rng);
}

template < class Predicate, class RNG >
entt::entity Deck::pop_if(Predicate f, size_t at_most, RNG& rng)
{
   auto indices = _find_indices(f, at_most);
   auto cards = pop_by_index(indices);
   shuffle(rng);
   return cards;
}

template < typename Container, typename >
std::vector< entt::entity > Deck::pop_by_index(Container indices)
{
   std::sort(indices.begin(), indices.end());
   if(not (indices.back() < size())) {
      throw std::out_of_range(
         "Indices to pop exceed spell container boundaries: Greatest index = "
         + std::to_string(indices.back()) + " > " + std::to_string(size()) + " = container size.");
   }
   std::vector< entt::entity > out;
   out.reserve(indices.size());
   size_t curr_idx = size() - 1;
   erase(std::remove_if(begin(), end(), [&](const auto& card) {
      bool match = (curr_idx == indices.back());
      curr_idx--;
      if(match) {
         indices.pop_back();
         out.emplace_back(card);
      }
      return match;
   }));
   return out;
}

template < typename IntType, typename >
std::vector< entt::entity > Deck::pop_by_index(std::initializer_list< IntType > indices)
{
   return pop_by_index< std::array< IntType, indices.size() > >(indices);
}

template < typename Predicate >
std::vector< entt::entity > Deck::_find_cards(Predicate f) const
{
   std::vector< entt::entity > cards;
   auto indices = _find_indices(f, size());
   // reserve the memory so that emplace_back doesn't trigger repeated memory relocation
   cards.reserve(indices.size());

   for(const auto& idx : indices) {
      cards.emplace_back(*this[idx]);
   }
   return cards;
}

template < typename RNG >
void Deck::shuffle_into(entt::entity card, size_t top_n, RNG&& rng)
{
   auto deck_size = size();
   if(top_n > deck_size) {
      top_n = deck_size;
   }
   auto max = std::min(top_n, deck_size);
   std::uniform_int_distribution< size_t > dist(0, max);
   auto pos = begin() + (deck_size - dist(std::forward< RNG >(rng)));
   insert(pos, card);
}

template < typename Predicate >
std::vector< entt::entity > Deck::find(Predicate f) const
{
   return _find_cards(f);
}

#endif  // LORAINE_DECK_H
