

#ifndef LORAINE_FIELDCARD_H
#define LORAINE_FIELDCARD_H

#include "cardbase.h"

/**
 * The base class for camp-based cards, i.e. cards that are played onto
 * the board and remain there to occupy space in the camp, until removed.
 * This class is mostly used to distinguish parameters for specific functions.
 */
class FieldCard: public Card {
  public:
   using Card::Card;  // use base class constructors
};

inline sptr< FieldCard > to_fieldcard(const sptr< Card >& card)
{
   return std::dynamic_pointer_cast< FieldCard >(card);
}

#endif  // LORAINE_FIELDCARD_H
