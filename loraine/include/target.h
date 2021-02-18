
#ifndef LORAINE_TARGET_H
#define LORAINE_TARGET_H

#include <optional>
#include <utility>

#include "engine/rulesets.h"
#include "utils/types.h"

class Card;

class Target {
  public:
   explicit Target(const sptr< Card >& card);
   explicit Target(Player player)
      : player(player), card()
   {
   }

   [[nodiscard]] bool is_nexus() const {return card.has_value();}
   [[nodiscard]] auto get_card() const {return card;}

  private:
   Player player;
   std::optional< sptr< Card > > card;
};

#endif  // LORAINE_TARGET_H
