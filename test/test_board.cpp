#include <gtest/gtest.h>

#include "all.h"
#include "test_cards.h"

TEST(BoardTest, BoardTest_Basics)
{
   using Battlefield = std::array< std::optional< sptr< Unit > >, BATTLEFIELD_SIZE >;
   using Camp = std::vector< sptr< Card > >;
   using CampQueue = std::queue< sptr< Card > >;

   auto unit1 = std::make_shared< TestUnit1 >(BLUE);
   auto unit2 = std::make_shared< TestUnit3 >(BLUE);
   auto unit3 = std::make_shared< TestUnit2 >(RED);
   auto unit4 = std::make_shared< TestUnit5 >(RED);
   auto unit5 = std::make_shared< TestUnit4 >(RED);
   Board board(
      {Battlefield{}, Battlefield{}},
      {Camp{unit1, unit2},
       Camp{unit3, unit4, unit5}},
      {CampQueue{}, CampQueue{}});

   EXPECT_EQ(board.count_units(BLUE, true), 2);
   EXPECT_EQ(board.count_units(RED, true), 3);

   for(std::shared_ptr<Card> unit : std::initializer_list<sptr<Card>>{unit1, unit2, unit3, unit4, unit5}) {
      auto [_, iter] = board.find_in_camp(to_unit(unit));
      EXPECT_EQ(*iter, unit);
   }
   board.remove_units({unit1, unit5});
   EXPECT_EQ(board.count_units(BLUE, true), 1);
   EXPECT_EQ(board.count_units(RED, true), 2);
}