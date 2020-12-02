#ifndef LORAINE_RULESETS_H
#define LORAINE_RULESETS_H

#include <stddef.h>

constexpr const static size_t MAX_CARD_COPIES_IN_DECK = 3;
constexpr const static size_t DECK_CARDS_LIMIT = 40;
constexpr const static size_t CHAMPIONS_LIMIT = 6;
constexpr const static size_t REGIONS_LIMIT = 2;

constexpr const static size_t BATTLEFIELD_SIZE = 6;
constexpr const static size_t CAMP_SIZE = 6;

constexpr const static size_t HAND_CARDS_LIMIT = 10;
constexpr const static size_t START_NEXUS_HEALTH = 20;
constexpr const static size_t SPELL_STACK_LIMIT = 10;
constexpr const static size_t MAX_MANA = 10;
constexpr const static size_t MAX_SPELL_MANA = 3;
constexpr const static size_t INITIAL_HAND_SIZE = 4;
constexpr const static size_t MAX_ROUNDS = 40;

enum Player { BLUE = 0, RED };

constexpr const static size_t n_players = static_cast< size_t >(Player::RED) + 1;

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

constexpr const static size_t n_status = static_cast< size_t >(Status::RED_WINS_CARD) + 1;

enum class Location {
   BATTLEFIELD = 0,
   BOARD,  // Board = Camp + Battlefield
   CAMP,
   DECK,
   EVERYWHERE,
   HAND,
   SPELLSTACK
};

constexpr const static size_t n_locations = static_cast< size_t >(Location::SPELLSTACK) + 1;

#endif  // LORAINE_RULESETS_H
