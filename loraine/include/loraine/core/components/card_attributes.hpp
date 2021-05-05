

#ifndef LORAINE_CARD_ATTRIBUTES_HPP
#define LORAINE_CARD_ATTRIBUTES_HPP

#include "loraine/core/gamedefs.h"
#include <entt/entt.hpp>

/**
 * Abstract base class (abc) for LOR cards.
 */
struct CardAttributes {
   /// the cards' name
   std::string name;
   /// the card's code in the LoR format, e.g. "DE01012"
   std::string code;
   /// the description text of the cards for their effects
   std::string description;
   /// text of the cards in context of the lol universe
   std::string lore;
   /// the region the cards is from
   Region region;
   /// the subgroup the cards belongs to
   Group group;
   /// the super type (champion, skill, none)
   CardSuperType super_type;
   /// the rarity of the cards
   Rarity rarity;
   /// whether the card is a spell, unit, landmark, or trap
   CardType card_type;
   /// whether a spell is collectible (i.e. can be added to a deck)
   bool is_collectible;
   /// was the card created by another
   std::optional< entt::entity > creator;
   /// whether the card is observable by all or only by its owner
   bool hidden;
};


#endif  // LORAINE_CARD_ATTRIBUTES_HPP
