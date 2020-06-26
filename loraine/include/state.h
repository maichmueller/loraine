
#ifndef LORAINE_STATE_H
#define LORAINE_STATE_H


#include <array>
#include <stack>
#include <vector>

#include "board.h"
#include "cards/card.h"
#include "deck.h"
#include "rulesets.h"
#include "types.h"

class State {
   SymArray< SID > m_nexus_ids = {0, 1};
   SymArray< i32 > m_nexus_health = {START_NEXUS_HEALTH, START_NEXUS_HEALTH};
   SymArray< u16 > m_mana = {0, 0};
   SymArray< u16 > m_spell_mana = {0, 0};

   SymArray< std::array< sptr<Card>, HAND_CARDS_LIMIT > > m_cards_in_hand;
   SymArray< Deck > m_deck;

   SymArray< bool > m_is_attacker;
   SymArray< bool > m_is_enlightened;
   SymArray< bool > m_can_plunder;
   SymArray< bool > m_has_attacked;

   u16 m_round = 0;

   // the number of dead allies per player
   SymArray< u16 > m_dead_ally_count = {0, 0};
   // the number of dead allies per player this round
   SymArray< u16 > m_dead_ally_count_rnd = {0, 0};

   SymArray< std::vector< Card > > m_graveyard;
   SymArray< std::vector< Card > > m_tossed_cards;
   sptr< Board > m_board;

   std::vector< Action > history;

  public:
    State();

    void shuffle_card_into_deck(sptr< Card> card);

};

#endif  // LORAINE_STATE_H
