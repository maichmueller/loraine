
#include "test_deck.h"

#include <gtest/gtest.h>

#include "core/deck.h"
#include "utils/random.h"
#include "test_cards.h"

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

   auto rng = random::create_rng(0);

   auto deck_copy = deck;
   auto deck_copy2 = deck;
   EXPECT_THROW(deck_copy.pop_by_index(deck.size()), std::out_of_range);
   EXPECT_THROW(deck_copy.pop_by_index({0UL, 1UL, 2UL, 3UL, 4UL, deck.size()}), std::out_of_range);

   auto card_at_0 = deck_copy2.at(0);
   auto card_at_2 = deck_copy2.at(2);
   auto popped = deck_copy2.pop_by_index({0, 2});
   EXPECT_EQ(card_at_0, card_at_0);
   EXPECT_EQ(card_at_2, card_at_2);

   auto card = deck_copy2.pop_by_code("CODE2", rng);
   EXPECT_EQ(card->immutables().code, "CODE2");
}