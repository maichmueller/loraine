
#include "test_deck.h"

#include <gtest/gtest.h>

#include "core/deck.h"
#include "test_cards.h"
#include "utils/random.h"

TEST(DeckTest, Functionalities)
{
   Deck deck({
      std::make_shared< TestUnit1 >(BLUE),
      std::make_shared< TestUnit1 >(BLUE),
      std::make_shared< TestUnit2 >(BLUE),
      std::make_shared< TestUnit2 >(BLUE),
      std::make_shared< TestUnit2 >(BLUE),
      std::make_shared< TestUnit3 >(BLUE),
      std::make_shared< TestUnit3 >(BLUE),
      std::make_shared< TestUnit3 >(BLUE),
      std::make_shared< TestUnit4 >(BLUE),
      std::make_shared< TestUnit4 >(BLUE),
      std::make_shared< TestUnit5 >(BLUE),
      std::make_shared< TestUnit5 >(BLUE),
   });

   // out of bounds checks

   auto deck_copy = deck;
   EXPECT_THROW(deck_copy.pop_by_index(deck.size()), std::out_of_range);
   EXPECT_THROW(deck_copy.pop_by_index({0UL, 1UL, 2UL, 3UL, 4UL, deck.size()}), std::out_of_range);

   // popping by index tests

   auto deck_copy2 = deck;
   auto card_at_0 = deck_copy2.at(0);
   auto card_at_2 = deck_copy2.at(2);
   auto popped = deck_copy2.pop_by_index({0, 2});
   EXPECT_EQ(card_at_0, card_at_0);
   EXPECT_EQ(card_at_2, card_at_2);

   // popping by code checks

   auto rng = random::create_rng(0);
   auto card = deck_copy2.pop_by_code("CODE2", rng);
   EXPECT_EQ(card->immutables().code, "CODE2");

   // filtering out specific cards with and without popping

   auto filtered = deck.find(
      [](const sptr< Card >& card) {
         if(auto unit = to_unit(card); utils::has_value(unit)) {
            return unit->power() == 4;
         }
         return false;
      },
      false);
   auto size_before = deck.size();
   EXPECT_EQ(filtered.size(), 3);
   EXPECT_TRUE(std::all_of(filtered.begin(), filtered.end(), [](const auto& unit) {
      return to_unit(unit)->power() == 4;
   }));
   auto filtered_popped = deck.find(
      [](const sptr< Card >& card) {
         if(auto unit = to_unit(card); utils::has_value(unit)) {
            return unit->power() == 4;
         }
         return false;
      },
      true);
   EXPECT_EQ(filtered_popped.size(), filtered.size());
   EXPECT_EQ(deck.size(), size_before - filtered.size());
}