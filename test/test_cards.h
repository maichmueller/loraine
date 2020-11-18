//
// Created by Michael on 08.10.20.
//

#ifndef LORAINE_TEST_CARDS_H
#define LORAINE_TEST_CARDS_H

#include "cards/card.h"

class TestUnit1: public Unit {
  public:
   TestUnit1(Player owner)
      : Unit(
      owner,
      "CODE1",
      "TestUnit1",
      "",
      "lore",
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

class TestUnit2: public Unit {
  public:
   TestUnit2(Player owner)
      : Unit(
      owner,
      "CODE2",
      "TestUnit2",
      "",
      "lore2",
      Region::NOXUS,
      Group::DRAGON,
          CardSuperType::NONE,
      Rarity::COMMON,
      true,
      5,
      4,
      5,
      {},
      {})
   {
   }
};

class TestUnit3: public Unit {
  public:
   TestUnit3(Player owner)
      : Unit(
      owner,
      "CODE3",
      "TestUnit3",
      "",
      "lore3",
      Region::NOXUS,
      Group::DRAGON,
          CardSuperType::NONE,
      Rarity::COMMON,
      true,
      3,
      3,
      3,
      {},
      {})
   {
   }
};

class TestUnit4: public Unit {
  public:
   TestUnit4(Player owner)
      : Unit(
      owner,
      "CODE4",
      "TestUnit4",
      "",
      "lore4",
      Region::TARGON,
      Group::NONE,
          CardSuperType::NONE,
      Rarity::EPIC,
      true,
      10,
      7,
      10,
      {},
      {})
   {
   }
};

class TestUnit5: public Unit {
  public:
   TestUnit5(Player owner)
      : Unit(
      owner,
      "CODE5",
      "TestUnit3",
      "",
      "lore5",
      Region::BILGEWATER,
      Group::NONE,
          CardSuperType::NONE,
      Rarity::RARE,
      true,
      8,
      7,
      7,
      {},
      {})
   {
   }
};

class TestUnit6: public Unit {
  public:
   TestUnit6(Player owner)
      : Unit(
      owner,
      "CODE5",
      "TestUnit6",
      "",
      "lore6",
      Region::PILTOVER_ZAUN,
      Group::NONE,
          CardSuperType::NONE,
      Rarity::RARE,
      true,
      1,
      1,
      2,
      {},
      {})
   {
   }
};

class TestSpell: public Spell {
  public:
   TestSpell(Player owner)
       : Spell(
          owner,
          "CODE5",
          "TestSpell",
          "",
          "lore7",
          Region::PILTOVER_ZAUN,
          Group::NONE,
          CardSuperType::NONE,
          Rarity::RARE,
          true,
          1,
          {},
          {})
   {
   }
};

#endif  // LORAINE_TEST_CARDS_H
