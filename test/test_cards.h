
#ifndef LORAINE_TEST_CARDS_H
#define LORAINE_TEST_CARDS_H

#include "cards/card.h"

class TestUnit1: public Unit {
  public:
   TestUnit1(Team owner)
       : Unit(
          Card::ConstState{
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
          Card::MutableState{owner, Location::DECK, 0, true},
          Unit::ConstUnitState{5, 4},
          Unit::MutableUnitState{5, 4})
   {
   }
};

class TestUnit2: public Unit {
  public:
   TestUnit2(Team owner)
       : Unit(
          Card::ConstState{
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
          Card::MutableState{owner, Location::DECK, 0, true},
          Unit::ConstUnitState{4, 5},
          Unit::MutableUnitState{4, 5})
   {
   }
};
class TestUnit3: public Unit {
  public:
   TestUnit3(Team owner)
       : Unit(
          Card::ConstState{
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
          Card::MutableState{owner, Location::DECK, 0, true},
          Unit::ConstUnitState{3, 4},
          Unit::MutableUnitState{3, 4})
   {
   }
};
class TestUnit4: public Unit {
  public:
   TestUnit4(Team owner)
       : Unit(
          Card::ConstState{
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
          Card::MutableState{owner, Location::DECK, 0, true},
          Unit::ConstUnitState{5, 4},
          Unit::MutableUnitState{5, 4})
   {
   }
};
class TestUnit5: public Unit {
  public:
   TestUnit5(Team owner)
       : Unit(
          Card::ConstState{
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
          Card::MutableState{owner, Location::DECK, 0, true},
          Unit::ConstUnitState{5, 4},
          Unit::MutableUnitState{5, 4})
   {
   }
};
class TestUnit6: public Unit {
  public:
   TestUnit6(Team owner)
       : Unit(
          Card::ConstState{
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
          Card::MutableState{owner, Location::DECK, 0, true},
          Unit::ConstUnitState{5, 4},
          Unit::MutableUnitState{5, 4})
   {
   }
};

class TestSpell: public Spell {
  public:
   TestSpell(Team owner)
       : Spell(
          Card::ConstState{
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
          Card::MutableState{owner, Location::DECK, 0, true})
   {
   }
};

#endif  // LORAINE_TEST_CARDS_H
