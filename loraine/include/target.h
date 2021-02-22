
#ifndef LORAINE_TARGET_H
#define LORAINE_TARGET_H

#include <optional>
#include <utility>

#include "engine/gamedefs.h"
#include "utils/types.h"

class Card;

class Target {
  public:
   explicit Target(const sptr< Card >& card);
   explicit Target(Team team)
      : team(team), card()
   {
   }

   [[nodiscard]] bool is_nexus() const {return card.has_value();}
   [[nodiscard]] auto get_card() const {return card;}

  private:
   Team team;
   std::optional< sptr< Card > > card;
};

#endif  // LORAINE_TARGET_H
