
#ifndef LORAINE_TARGET_H
#define LORAINE_TARGET_H

#include <utility>

#include "rulesets.h"
#include "types.h"

class Card;

struct Target {
   bool is_nexus;
   Player player;
   Location loc;
   size_t index;
   sptr< Card > card;

   Target(bool is_nexus, Player player, Location loc, size_t index, sptr< Card > card)
       : is_nexus(is_nexus), player(player), loc(loc), index(index), card(std::move(card))
   {
   }
   explicit Target(Player player)
       : is_nexus(true), player(player), loc(Location::EVERYWHERE), index(0), card(nullptr)
   {
   }
};



#endif  // LORAINE_TARGET_H
