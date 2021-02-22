
#ifndef LORAINE_NEXUS_H
#define LORAINE_NEXUS_H


#include "gamedefs.h"

class Nexus {
   Nexus(Team team, long health) : m_team(team), m_health(health) {}

   [[nodiscard]] inline auto team() const {return m_team;}
   inline void change_health(long health) { m_health += health;}

  private:
   Team m_team;
   long m_health;
};

#endif  // LORAINE_NEXUS_H
