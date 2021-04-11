#include <gtest/gtest.h>

#include "all.h"
#include "test_cards.h"

TEST(BoardTest, Functionalities)
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

   EXPECT_EQ(board.count_occupied_spots(BLUE, true), 2);
   EXPECT_EQ(board.count_occupied_spots(RED, true), 3);

   board.add_to_bf(unit1, 2);
   auto& bf_blue =  board.battlefield(Team::BLUE);
   EXPECT_EQ(bf_blue[0], nullptr );
   EXPECT_EQ(bf_blue[1], nullptr );
   EXPECT_EQ(bf_blue[2], unit1 );
   auto unit6 = std::make_shared<TestUnit5>(BLUE);
   board.add_to_camp(unit6);
   auto& camp_blue =  board.camp(Team::BLUE);
   EXPECT_EQ(camp_blue.back(), unit6);
}