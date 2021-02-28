
#ifndef LORAINE_LANDMARK_H
#define LORAINE_LANDMARK_H

#include "fieldcard.h"

class Landmark: public FieldCard {
  public:
   // use base class constructors
   using FieldCard::FieldCard;

};

inline sptr< Landmark > to_landmark(const sptr< Card >& card)
{
   return std::dynamic_pointer_cast< Landmark >(card);
}

#endif  // LORAINE_LANDMARK_H
