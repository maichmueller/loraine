
#ifndef LORAINE_TARGET_H
#define LORAINE_TARGET_H

#include <utility>

#include "rulesets.h"
#include "types.h"

class Card;

struct Target {
   bool is_nexus;
   Player player;
   Location location;
   size_t index;
   std::optional< sptr< Card > > opt_card;

   Target(
      bool is_nexus,
      Player player,
      Location loc,
      size_t index,
      std::optional< sptr< Card > > card = {})
       : is_nexus(is_nexus), player(player), location(loc), index(index), opt_card(std::move(card))
   {
   }
   explicit Target(Player player)
       : is_nexus(true), player(player), location(Location::EVERYWHERE), index(0), opt_card()
   {
   }
};

#endif  // LORAINE_TARGET_H
