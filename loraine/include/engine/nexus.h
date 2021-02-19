
#ifndef LORAINE_NEXUS_H
#define LORAINE_NEXUS_H


#include "rulesets.h"

class Nexus {
   Nexus(Player player, long health) : m_player(player), m_health(health) {}

   [[nodiscard]] inline auto player() const {return m_player;}
   inline void change_health(long health) { m_health += health;}

  private:
   Player m_player;
   long m_health;
};

#endif  // LORAINE_NEXUS_H
