#ifndef LORAINE_RULESETS_H
#define LORAINE_RULESETS_H


enum Player {
   BLUE = 0,
   RED
};

#define INDIVIDUAL_CARD_LIMIT 3u
#define DECK_CARDS_LIMIT 40u
#define CHAMPIONS_LIMIT 6u
#define REGIONS_LIMIT 2u
#define INITIAL_HAND_SIZE 4u

#define ROUND_LIMIT 40u  // after this many rounds it is an auto-tie

#define BATTLEFIELD_SIZE 6u
#define CAMP_SIZE 6u

#define HAND_CARDS_LIMIT 10u
#define BOARD_CARDS_LIMIT 6u

#define START_NEXUS_HEALTH 20u

#define SPELL_STACK_LIMIT 10u
#define MAX_MANA 10u
#define MAX_SPELL_MANA 3u

#define MAX_ROUNDS 40u

#define ONGOING 404u
#define TIE 0u
#define BLUE_WINS_NEXUS 1u
#define BLUE_WINS_DRAW 2u
#define BLUE_WINS_CARD 3u
#define RED_WINS_NEXUS 4u
#define RED_WINS_DRAW 5u
#define RED_WINS_CARD 6u

#endif  // LORAINE_RULESETS_H
