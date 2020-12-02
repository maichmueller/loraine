#ifndef LORAINE_CARD_DEFS_H
#define LORAINE_CARD_DEFS_H

#include "types.h"

enum struct Rarity { NONE = 0, CHAMPION, COMMON, EPIC, RARE };

constexpr const static size_t n_rarities = static_cast< size_t >(Rarity::RARE) + 1;

enum struct Group {
   NONE = 0,
   DRAGON,
   ELITE,
   ELNUK,
   PORO,
   SEA_MONSTER,
   SPIDER,
   TECH,
   TREASURE,
   YETI
};

constexpr const static size_t n_groups = static_cast< size_t >(Group::YETI) + 1;

enum struct CardType { SPELL, UNIT, LANDMARK };

constexpr const static size_t n_cardtypes = static_cast< size_t >(CardType::LANDMARK) + 1;

enum struct CardSuperType { NONE, CHAMPION, SKILL };

constexpr const static size_t n_cardsupertypes = static_cast< size_t >(CardSuperType::SKILL) + 1;

enum struct Region {
   BILGEWATER = 0,
   DEMACIA,
   FRELJORD,
   IONIA,
   NOXUS,
   PILTOVER_ZAUN,
   SHADOW_ISLES,
   TARGON,
};

constexpr const static size_t n_regions = static_cast< size_t >(Region::TARGON) + 1;

enum class Keyword {
   ALLEGIANCE = 0,  // define the starting value, necessary for indexing.
   ATTACK,
   ATTUNE,
   BARRIER,
   BEHOLD,
   BURST,
   CANT_BLOCK,
   CAPTURE,
   CHALLENGER,
   DYBREAK,
   DEEP,
   DOUBLE_ATTACK,
   DRAIN,
   ELUSIVE,
   ENLIGHTENED,
   EPHEMERAL,
   FAST,
   FEARSOME,
   FLEETING,
   FROSTBITE,
   FURY,
   NEXUS_STRIKE,
   NIGHTFALL,
   OBLITERATE,
   IMMOBILE,
   INVOKE,
   LANDMARK,
   LAST_BREATH,
   LIFESTEAL,
   OVERWHELM,
   PLUNDER,
   QUICK_ATTACK,
   RECALL,
   REGENERATION,
   SCOUT,
   SKILL,
   SLOW,
   SPELLSHIELD,
   STRIKE,
   STRONGEST,
   STUN,
   SUPPORT,
   TOSS,
   TOUGH,
   TRAP,
   VULNERABLE,
   WEAKEST,
};

constexpr const static size_t n_keywords = static_cast< size_t >(Keyword::WEAKEST);

// This is intended to be used as lookup table for keywords:
// Does the card have the keyword 'lifesteal'?
// -> m_keywords[LIFESTEAL] will return the boolean answer
using KeywordMap = std::array< bool, n_keywords >;

KeywordMap create_kword_list(std::initializer_list< Keyword > kwords);

#endif  // LORAINE_CARD_DEFS_H
