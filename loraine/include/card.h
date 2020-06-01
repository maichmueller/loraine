//
// Created by michael on 29.05.20.
//

#ifndef LORAINE_CARD_H
#define LORAINE_CARD_H

#include "keywords.h"
#include "region.h"
#include "types.h"

class State;

enum struct Rarity { NONE, COMMON, RARE, EPIC, CHAMPION };

enum struct Group {
   NONE,
   ELITE,
   ELNUK,
   PORO,
   SEA_MONSTER,
   SPIDER,
   TECH,
   TREASURE,
   YETI,
};

enum struct UnitType { NONE, FOLLOWER, CHAMPION };

/*
 * Abstract base class (abc) for LOR cards.
 */
template < size_t N >
class Card {
  protected:
   // the card's name
   const char* const m_name;
   // the description text of the card
   const char* const m_effect_desc;
   // text of the card in context of the lol universe
   const char* const m_lore;
   // the mana it costs to play the card
   u64 m_mana_cost;
   // the region the card is from
   Region m_region;
   // the subgroup the card belongs to
   Group m_group;
   // the unit type (champion, follower or spell/skill=none)
   UnitType m_unit_type;
   // all the keywords pertaining to the card
   std::array< Keyword, N > m_keywords;
   // the rarity of the card
   Rarity m_rarity;

  public:
   // the card's effect function, purely virtual
   virtual void effect(State& state) = 0;
   // the general constructor for initializing the members
   Card(
      const char* name,
      const char* effect_desc,
      const char* lore,
      u64 mana_cost,
      Region region,
      Group group,
      UnitType unit_type,
      std::array< Keyword, N > keywords,
      Rarity rarity)
       : m_name(name),
         m_effect_desc(effect_desc),
         m_lore(lore),
         m_mana_cost(mana_cost),
         m_region(region),
         m_group(group),
         m_unit_type(unit_type),
         m_keywords(std::move(keywords)),
         m_rarity(rarity)
   {
   }
};

template < size_t N >
class Spell: public Card< N > {
};

template < size_t N >
class Unit: public Card< N > {
  protected:
   u64 m_damage;
   u64 m_health;
};

template < size_t N >
class Champion: public Unit< N > {
};

template < size_t N >
class Follower: public Unit< N > {
};

#endif  // LORAINE_CARD_H
