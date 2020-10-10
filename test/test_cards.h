//
// Created by Michael on 08.10.20.
//

#ifndef LORAINE_TEST_CARDS_H
#define LORAINE_TEST_CARDS_H

#include "cards/card.h"

class TestCard1: public Unit {
  public:
   TestCard1()
       : Unit(
          BLUE,
          "CODE1",
          "TestUnit1",
          "",
          "Lore Lore lore",
          Region::DEMACIA,
          Group::ELITE,
          CardSuperType::NONE,
          Rarity::COMMON,
          true,
          2,
          4,
          1,
          {},
          {})
   {
   }
};

#endif  // LORAINE_TEST_CARDS_H
