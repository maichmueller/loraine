#ifndef LORAINE_RULESETS_H
#define LORAINE_RULESETS_H

#include <cstddef>

enum Player { BLUE = 0, RED };

inline Player opponent(Player player)
{
   return Player(1 - static_cast< uint16_t >(player));
}

// class Player {
//   size_t val;
//
//   static Player init_player(size_t v);
//
//  public:
//   explicit Player(size_t v) : val(v) {}
//
//   static const Player BLUE = init_player(0);
//   static const Player RED = init_player(1);
//
//};

constexpr const size_t n_players = static_cast< size_t >(Player::RED) + 1;

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

#endif  // LORAINE_RULESETS_H
