//
// Created by michael on 30.05.20.
//

#ifndef LORAINE_DECK_H
#define LORAINE_DECK_H

#include <functional>
#include <map>
#include <vector>

#include "cards/card.h"
#include "types.h"

/*
 * Deck requirements:
 * - Pop efficiently at end
 * - draw any category of cards individually (spell, champion, follower,...)
 * -
 */

class Deck {
   // vectors of sptrs should be fastest when cache locality holds
   std::vector< sptr< Card > > m_cards;
   // champions are also stored separately as they have level up conditions,
   // which need to be checked after every action.
   std::vector< sptr< Card > > m_champions;

  public:
   /*
    * Method to filter out a specific (type of) spell
    * as decided by the filter
    */
   std::vector< u32 > find_indices(
      const std::function< bool(sptr< Card >) >& filter);
   /*
    * Method to filter out a specific (type of) spell
    * as decided by the filter
    */
   std::vector< sptr< Card > > find_spell(
      const std::function< bool(sptr< Card >) >& filter);
};

#endif  // LORAINE_DECK_H
