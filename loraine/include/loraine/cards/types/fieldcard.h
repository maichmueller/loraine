

#ifndef LORAINE_FIELDCARD_H
#define LORAINE_FIELDCARD_H

#include "cardbase.h"

/**
 * The base class for camp-based cards, i.e. cards that are played onto
 * the board and remain there to occupy space in the camp, until removed.
 * This class is mostly used to distinguish parameters for specific functions.
 */
class FieldCard: public Cloneable< abstract_method< FieldCard >, inherit_constructors< Card > > {
  public:
   using Cloneable::Cloneable;  // use base class Card constructors
};

inline sptr< FieldCard > to_fieldcard(const sptr< Card >& card)
{
   return std::dynamic_pointer_cast< FieldCard >(card);
}

//class Corpse final: public Cloneable< Corpse, inherit_constructors< FieldCard > > {
//  private:
//   Corpse(Team team)
//       : Cloneable(
//          ConstState{
//             "CORPSE",
//             "CORPSE",
//             "CORPSE",
//             "CORPSE",
//             Region::DEMACIA,
//             Group::NONE,
//             CardSuperType::NONE,
//             Rarity::NONE,
//             CardType::UNIT,
//             0,
//             false},
//          MutableState{team, Location::BATTLEFIELD, 0, false}){};
//
//  public:
//   Corpse(Corpse const&) = delete;
//   void operator=(Corpse const&) = delete;
//
//   static sptr< Corpse > get(Team team)
//   {
//      // one corpse for each team
//      static SymArr< sptr< Corpse > > instances = {
//         sptr< Corpse >(new Corpse(Team(0))), sptr< Corpse >(new Corpse(Team(1)))};
//      return instances[static_cast< size_t >(team)];
//   }
//};

#endif  // LORAINE_FIELDCARD_H
