#include <gtest/gtest.h>

#include "all.h"
#include "test_cards.h"

TEST(BoardTest, BoardTest_Basics)
{
   using Battlefield = std::array< std::optional< sptr< Unit > >, BATTLEFIELD_SIZE >;
   using Camp = std::vector< sptr< Unit > >;
   using CampQueue = std::queue< sptr< Unit > >;

   Board board(
      {Battlefield{}, Battlefield{}},
      {Camp{std::make_shared<TestCard1>()},
       Camp{std::make_shared<TestCard1>()}},
      {CampQueue{}, CampQueue{}});

//   EXPECT_EQ();
}