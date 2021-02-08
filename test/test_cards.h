//
// Created by Michael on 08.10.20.
//

#ifndef LORAINE_TEST_CARDS_H
#define LORAINE_TEST_CARDS_H

#include "cards/card_types/all_card_types.h"

class TestUnit1: public Unit {
  public:
   TestUnit1(Player owner)
       : Unit(
          Unit::ConstUnitData{5, 4},
          Unit::MutableUnitData{5, 4},
          Card::ConstData{
             "CODE1",
             "TestUnit1",
             "",
             "lore",
             Region::DEMACIA,
             Group::ELITE,
             CardSuperType::NONE,
             Rarity::COMMON,
             CardType::UNIT,
             2,
          },
          Card::MutableData{owner, Location::DECK,0, true})
   {
   }
};

class TestUnit2: public Unit {
  public:
   TestUnit2(Player owner)
       : Unit(
          Unit::ConstUnitData{4, 5},
          Unit::MutableUnitData{4, 5},
          Card::ConstData{
             "CODE2",
             "TestUnit2",
             "",
             "lore",
             Region::DEMACIA,
             Group::ELITE,
             CardSuperType::NONE,
             Rarity::COMMON,
             CardType::UNIT,
             2,
          },
          Card::MutableData{owner, Location::DECK,0, true})
   {
   }
};
class TestUnit3: public Unit {
  public:
   TestUnit3(Player owner)
       : Unit(
          Unit::ConstUnitData{3, 4},
          Unit::MutableUnitData{3, 4},
          Card::ConstData{
             "CODE3",
             "TestUnit3",
             "",
             "lore",
             Region::DEMACIA,
             Group::ELITE,
             CardSuperType::NONE,
             Rarity::COMMON,
             CardType::UNIT,
             2,
          },
          Card::MutableData{owner, Location::DECK,0, true})
   {
   }
};
class TestUnit4: public Unit {
  public:
   TestUnit4(Player owner)
       : Unit(
          Unit::ConstUnitData{5, 4},
          Unit::MutableUnitData{5, 4},
          Card::ConstData{
             "CODE4",
             "TestUnit4",
             "",
             "lore",
             Region::DEMACIA,
             Group::ELITE,
             CardSuperType::NONE,
             Rarity::COMMON,
             CardType::UNIT,
             2,
          },
          Card::MutableData{owner, Location::DECK,0, true})
   {
   }
};
class TestUnit5: public Unit {
  public:
   TestUnit5(Player owner)
       : Unit(
          Unit::ConstUnitData{5, 4},
          Unit::MutableUnitData{5, 4},
          Card::ConstData{
             "CODE5",
             "TestUnit5",
             "",
             "lore",
             Region::DEMACIA,
             Group::ELITE,
             CardSuperType::NONE,
             Rarity::COMMON,
             CardType::UNIT,
             2,
          },
          Card::MutableData{owner, Location::DECK,0, true})
   {
   }
};
class TestUnit6: public Unit {
  public:
   TestUnit6(Player owner)
       : Unit(
          Unit::ConstUnitData{5, 4},
          Unit::MutableUnitData{5, 4},
          Card::ConstData{
             "CODE6",
             "TestUnit6",
             "",
             "lore",
             Region::DEMACIA,
             Group::ELITE,
             CardSuperType::NONE,
             Rarity::COMMON,
             CardType::UNIT,
             2,
          },
          Card::MutableData{owner, Location::DECK,0, true})
   {
   }
};

class TestSpell: public Spell {
  public:
   TestSpell(Player owner)
       : Spell(
          Card::ConstData{
             "CODE6",
             "TestUnit6",
             "",
             "lore",
             Region::PILTOVER_ZAUN,
             Group::NONE,
             CardSuperType::NONE,
             Rarity::RARE,
             CardType::SPELL,
             1,
          },
          Card::MutableData{owner, Location::DECK,0, true})
   {
   }
};

#endif  // LORAINE_TEST_CARDS_H
