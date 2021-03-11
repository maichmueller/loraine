#ifndef LORAINE_GAMEDEFS_H
#define LORAINE_GAMEDEFS_H

#include <cstddef>
#include <cstdint>

enum Team { BLUE = 0, RED };

inline Team opponent(Team team)
{
   return Team(1 - static_cast< uint16_t >(team));
}


constexpr const size_t n_teams = static_cast< size_t >(Team::RED) + 1;

enum Status {
   TIE = 0,
   ONGOING,
   BLUE_WINS_NEXUS,
   BLUE_WINS_DRAW,
   BLUE_WINS_CARD,
   RED_WINS_NEXUS,
   RED_WINS_DRAW,
   RED_WINS_CARD
};

constexpr const size_t n_status = static_cast< size_t >(Status::RED_WINS_CARD) + 1;

enum class Location {
   BATTLEFIELD = 0,
   CAMP,
   BOARD,  // Board = Camp + Battlefield
   DECK,
   GRAVEYARD,
   TOSSED,
   HAND,
   SPELLSTACK,
};

constexpr const size_t n_locations = static_cast< size_t >(Location::SPELLSTACK) + 1;

#endif  // LORAINE_GAMEDEFS_H
