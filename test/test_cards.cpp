#include "test_cards.h"

#include <gtest/gtest.h>

TEST(CardTest, CardTest_Basics)
{
   auto unit1 = std::make_shared< TestUnit1 >(BLUE);
   auto unit2 = std::make_shared< TestUnit2 >(BLUE);
   auto unit3 = std::make_shared< TestUnit3 >(RED);
   auto spell1 = std::make_shared< TestSpell> (BLUE);
   auto spell2 = std::make_shared< TestSpell> (RED);

   EXPECT_EQ(spell1->is_spell(), true);
   EXPECT_EQ(unit1->is_unit(), true);
   EXPECT_EQ(spell1->is_landmark(), false);
   EXPECT_EQ(unit1->is_landmark(), false);
   EXPECT_EQ(unit2->power(), 4);
   EXPECT_EQ(unit3->power(), 3);
   EXPECT_EQ(unit2->health(), 5);

   unit2->take_damage(2);
   EXPECT_EQ(unit2->unit_mutables().damage, 2);
   EXPECT_EQ(unit2->health(), 3);
   EXPECT_EQ(unit2->unit_mutables().health_delta, 0);
   EXPECT_EQ(unit2->unit_mutables().health_base, 5);

   unit2->heal(5);
   EXPECT_EQ(unit2->unit_mutables().health_delta, 0);
   EXPECT_EQ(unit2->unit_mutables().damage, 0);

   unit2->add_health(10, true);
   EXPECT_EQ(unit2->unit_mutables().health_base, 15);
   EXPECT_EQ(unit2->unit_mutables().health_delta, 0);
   EXPECT_EQ(unit2->health(), 15);

   unit2->add_health(-10, true);
   EXPECT_EQ(unit2->unit_mutables().health_base, 5);
   EXPECT_EQ(unit2->unit_mutables().health_delta, 0);
   EXPECT_EQ(unit2->health(), 5);

   unit2->add_health(10, false);
   EXPECT_EQ(unit2->unit_mutables().health_base, 5);
   EXPECT_EQ(unit2->unit_mutables().health_delta, 10);
   EXPECT_EQ(unit2->health(), 15);

   unit2->add_health(-10, false);
   EXPECT_EQ(unit2->unit_mutables().health_base, 5);
   EXPECT_EQ(unit2->unit_mutables().health_delta, 0);
   EXPECT_EQ(unit2->health(), 5);

   unit2->add_health(10, false);
   unit2->take_damage(10);
   EXPECT_EQ(unit2->unit_mutables().health_base, 5);
   EXPECT_EQ(unit2->unit_mutables().health_delta, 10);
   EXPECT_EQ(unit2->unit_mutables().damage, 10);
   EXPECT_EQ(unit2->health(), 5);

   unit2->add_keyword(Keyword::EPHEMERAL);
   EXPECT_EQ(unit2->has_keyword(Keyword::EPHEMERAL), true);
   unit2->remove_keyword(Keyword::EPHEMERAL);
   EXPECT_EQ(unit2->has_keyword(Keyword::EPHEMERAL), false);

//   EXPECT_EQ(unit2->check_play_condition(), true);

}