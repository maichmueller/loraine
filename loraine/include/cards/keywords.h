//
// Created by michael on 31.05.20.
//

#ifndef LORAINE_KEYWORDS_H
#define LORAINE_KEYWORDS_H

#include <array>

enum class Keyword {
   ALLEGIANCE = 0,  // define the starting value, necessary for indexing.
   ATTUNE,
   BARRIER,
   BURST,
   CANT_BLOCK,
   CAPTURE,
   CHALLENGER,
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
   NEXUS_STRIKE,
   OBLITERATE,
   IMMOBILE,
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

constexpr const static size_t n_keywords = static_cast<size_t>(Keyword::WEAKEST);

// This is intended to be used as lookup table for keywords:
// Does the card have the keyword 'lifesteal'?
// -> m_keywords[LIFESTEAL] will return the boolean answer
using KeywordMap = std::array< bool , static_cast<int>(Keyword::WEAKEST)>;

KeywordMap create_kword_list(std::initializer_list< Keyword > kwords);

#endif  // LORAINE_KEYWORDS_H
