

#ifndef LORAINE_TARGET_H
#define LORAINE_TARGET_H

#include "card.h"
#include "rulesets.h"

class Target {
  public:
   bool is_nexus;
   Player player;
   Location loc;
   size_t index;
   sptr<Card> card;
};

#endif  // LORAINE_TARGET_H
