#include <gtest/gtest.h>

#include "all.h"
#include "test_cards.h"

TEST(BoardTest, BoardTest_Basics)
{
   using bf_type = Board::BfType;
   using camp_type = Board::CampType;
   using camp_queue_type = Board::CampQueueType;
   auto unit1 = std::make_shared< TestUnit1 >(BLUE);
   auto unit2 = std::make_shared< TestUnit3 >(BLUE);
   auto unit3 = std::make_shared< TestUnit2 >(RED);
   auto unit4 = std::make_shared< TestUnit5 >(RED);
   auto unit5 = std::make_shared< TestUnit4 >(RED);
   Board board(
      6,
      6,
      {bf_type{}, bf_type{}},
      {camp_type{unit1, unit2}, camp_type{unit3, unit4, unit5}});

   EXPECT_EQ(board.count_units(BLUE, true), 2);
   EXPECT_EQ(board.count_units(RED, true), 3);
}