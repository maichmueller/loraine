
#ifndef LORAINE_LANDMARK_H
#define LORAINE_LANDMARK_H

#include "fieldcard.h"

class Landmark: public Cloneable<Landmark, inherit_constructors<FieldCard>> {
  public:
   // use base class constructors
   using Cloneable::Cloneable;

};

inline sptr< Landmark > to_landmark(const sptr< Card >& card)
{
   return std::dynamic_pointer_cast< Landmark >(card);
}

#endif  // LORAINE_LANDMARK_H
