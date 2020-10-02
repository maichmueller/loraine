#ifndef LORAINE_RULESETS_H
#define LORAINE_RULESETS_H

enum Player { BLUE = 0, RED };

constexpr const size_t MAX_CARD_COPIES_IN_DECK = 3;
constexpr const size_t DECK_CARDS_LIMIT = 40;
constexpr const size_t CHAMPIONS_LIMIT = 6;
constexpr const size_t REGIONS_LIMIT = 2;

constexpr const size_t BATTLEFIELD_SIZE = 6;
constexpr const size_t CAMP_SIZE = 6;

constexpr const size_t HAND_CARDS_LIMIT = 10;
constexpr const size_t START_NEXUS_HEALTH = 20;
constexpr const size_t SPELL_STACK_LIMIT = 10;
constexpr const size_t MAX_MANA = 10;
constexpr const size_t MAX_SPELL_MANA = 3;
constexpr const size_t INITIAL_HAND_SIZE = 4;
constexpr const size_t MAX_ROUNDS = 40;

enum Status {
   TIE = 100,
   ONGOING,
   BLUE_WINS_NEXUS,
   BLUE_WINS_DRAW,
   BLUE_WINS_CARD,
   RED_WINS_NEXUS,
   RED_WINS_DRAW,
   RED_WINS_CARD
};

enum class Location {
   EVERYWHERE = 0,
   BOARD,  // Board = Camp + Battlefield
   HAND,
   CAMP,
   DECK,
   BATTLEFIELD,
};

#endif  // LORAINE_RULESETS_H
