
#ifndef LORAINE_LANDMARK_H
#define LORAINE_LANDMARK_H

#include "card.h"

class Landmark: public Card {
  public:
   // use base class constructors
   using Card::Card;

   [[nodiscard]] bool is_landmark() const override { return true; }
   [[nodiscard]] bool is_unit() const override { return false; }
};

inline sptr< Landmark > to_landmark(const sptr< Card >& card)
{
   return std::dynamic_pointer_cast< Landmark >(card);
}

#endif  // LORAINE_LANDMARK_H
