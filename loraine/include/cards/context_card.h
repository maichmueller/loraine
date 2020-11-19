
#ifndef LORAINE_CONEXT_CARD_H
#define LORAINE_CONEXT_CARD_H

#include "rulesets.h"
#include "types.h"

class Card;

struct ContextCard {
   ContextCard(Location location, size_t i, const sptr< Card >& card)
       : location(location), index(i), card(card)
   {
   }
   Location location;
   size_t index;
   sptr< Card > card;
};

#endif  // LORAINE_CONEXT_CARD_H
