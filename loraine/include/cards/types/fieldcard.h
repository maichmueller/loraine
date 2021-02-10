

#ifndef LORAINE_FIELDCARD_H
#define LORAINE_FIELDCARD_H

#include "cardbase.h"

/*
 * The base class for camp-based cards, i.e. cards that are played onto
 * the board and remain there to occupy space in the camp, until removed.
 */
class FieldCard: public Card {
  public:
   using Card::Card;  // use base class constructors

   struct MutableFieldCardData {
      long index = -1;  // the index in the camp this card occupies
   };

   [[nodiscard]] inline auto& fieldcard_mutable_attrs() const { return m_fieldcard_mutables; }
   [[nodiscard]] inline auto& fieldcard_mutable_attrs() { return m_fieldcard_mutables; }

  private:
   MutableFieldCardData m_fieldcard_mutables;
};

inline sptr< FieldCard > to_fieldcard(const sptr< Card >& card)
{
   return std::dynamic_pointer_cast< FieldCard >(card);
}

#endif  // LORAINE_FIELDCARD_H
