

#ifndef LORAINE_CONFIG_H
#define LORAINE_CONFIG_H

#include <filesystem>
#include <string>

#include "nexus.h"

struct Config {
   static Config load(std::filesystem::path filepath);
   void save(std::filesystem::path filepath);

   const size_t MAX_CARD_COPIES_IN_DECK = 3;
   const size_t DECK_CARDS_LIMIT = 40;
   const size_t CHAMPIONS_LIMIT = 6;
   const size_t REGIONS_LIMIT = 2;
   const size_t BATTLEFIELD_SIZE = 6;
   const size_t CAMP_SIZE = 6;
   const size_t HAND_CARDS_LIMIT = 10;
   const size_t START_NEXUS_HEALTH = 20;
   const size_t SPELL_STACK_LIMIT = 10;
   const size_t MAX_MANA = 10;
   const size_t MAX_FLOATING_MANA = 3;
   const size_t MANA_START = 0;
   const size_t FLOATING_MANA_START = 0;
   const size_t INITIAL_HAND_SIZE = 4;
   const size_t MAX_ROUNDS = 40;

   const Nexus::EffectMap PASSIVE_POWERS_BLUE = {};
   const Nexus::EffectMap PASSIVE_POWERS_RED = {};
   const KeywordMap NEXUS_KEYWORDS_BLUE = {};
   const KeywordMap NEXUS_KEYWORDS_RED = {};
};

#endif  // LORAINE_CONFIG_H
