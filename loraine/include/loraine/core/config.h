

#ifndef LORAINE_CONFIG_H
#define LORAINE_CONFIG_H

#include <filesystem>
#include <string>

struct Config {
   Config() { _set_default(); }
   Config(
      size_t max_card_copies_in_deck,
      size_t deck_cards_limit,
      size_t champions_limit,
      size_t regions_limit,
      size_t battlefield_size,
      size_t camp_size,
      size_t hand_cards_limit,
      size_t start_nexus_health,
      size_t spell_stack_limit,
      size_t max_mana,
      size_t max_floating_mana,
      size_t mana_start,
      size_t floating_mana_start,
      size_t initial_hand_size,
      size_t max_rounds,
      size_t invalid_actions_limit,
      size_t enlightenment_threshold)
       : MAX_CARD_COPIES_IN_DECK(max_card_copies_in_deck),
         DECK_CARDS_LIMIT(deck_cards_limit),
         CHAMPIONS_LIMIT(champions_limit),
         REGIONS_LIMIT(regions_limit),
         BATTLEFIELD_SIZE(battlefield_size),
         CAMP_SIZE(camp_size),
         HAND_CARDS_LIMIT(hand_cards_limit),
         START_NEXUS_HEALTH(start_nexus_health),
         SPELL_STACK_LIMIT(spell_stack_limit),
         MAX_MANA(max_mana),
         MAX_FLOATING_MANA(max_floating_mana),
         MANA_START(mana_start),
         FLOATING_MANA_START(floating_mana_start),
         INITIAL_HAND_SIZE(initial_hand_size),
         MAX_ROUNDS(max_rounds),
         INVALID_ACTIONS_LIMIT(invalid_actions_limit),
         ENLIGHTENMENT_THRESHOLD(enlightenment_threshold)
   {
   }

   static Config load(std::filesystem::path filepath);
   void save(std::filesystem::path filepath);

   size_t MAX_CARD_COPIES_IN_DECK;
   size_t DECK_CARDS_LIMIT;
   size_t CHAMPIONS_LIMIT;
   size_t REGIONS_LIMIT;
   size_t BATTLEFIELD_SIZE;
   size_t CAMP_SIZE;
   size_t HAND_CARDS_LIMIT;
   size_t START_NEXUS_HEALTH;
   size_t SPELL_STACK_LIMIT;
   size_t MAX_MANA;
   size_t MAX_FLOATING_MANA;
   size_t MANA_START;
   size_t FLOATING_MANA_START;
   size_t INITIAL_HAND_SIZE;
   size_t MAX_ROUNDS;
   size_t INVALID_ACTIONS_LIMIT;
   size_t ENLIGHTENMENT_THRESHOLD;

   const std::vector< std::string > PASSIVE_POWERS_BLUE = {};
   const std::vector< std::string > PASSIVE_POWERS_RED = {};
   const KeywordMap NEXUS_KEYWORDS_BLUE = {};
   const KeywordMap NEXUS_KEYWORDS_RED = {};

   constexpr static size_t DEF_MAX_CARD_COPIES_IN_DECK = 3;
   constexpr static size_t DEF_DECK_CARDS_LIMIT = 40;
   constexpr static size_t DEF_CHAMPIONS_LIMIT = 6;
   constexpr static size_t DEF_REGIONS_LIMIT = 2;
   constexpr static size_t DEF_BATTLEFIELD_SIZE = 6;
   constexpr static size_t DEF_CAMP_SIZE = 6;
   constexpr static size_t DEF_HAND_CARDS_LIMIT = 10;
   constexpr static size_t DEF_START_NEXUS_HEALTH = 20;
   constexpr static size_t DEF_SPELL_STACK_LIMIT = 10;
   constexpr static size_t DEF_MAX_MANA = 10;
   constexpr static size_t DEF_MAX_FLOATING_MANA = 3;
   constexpr static size_t DEF_MANA_START = 0;
   constexpr static size_t DEF_FLOATING_MANA_START = 0;
   constexpr static size_t DEF_INITIAL_HAND_SIZE = 4;
   constexpr static size_t DEF_MAX_ROUNDS = 40;
   constexpr static size_t DEF_INVALID_ACTIONS_LIMIT = 40;
   constexpr static size_t DEF_ENLIGHTENMENT_THRESHOLD = 10;

  private:
   void _set_default()
   {
      MAX_CARD_COPIES_IN_DECK = DEF_MAX_CARD_COPIES_IN_DECK;
      DECK_CARDS_LIMIT = DEF_DECK_CARDS_LIMIT;
      CHAMPIONS_LIMIT = DEF_CHAMPIONS_LIMIT;
      REGIONS_LIMIT = DEF_REGIONS_LIMIT;
      BATTLEFIELD_SIZE = DEF_BATTLEFIELD_SIZE;
      CAMP_SIZE = DEF_CAMP_SIZE;
      HAND_CARDS_LIMIT = DEF_HAND_CARDS_LIMIT;
      START_NEXUS_HEALTH = DEF_START_NEXUS_HEALTH;
      SPELL_STACK_LIMIT = DEF_SPELL_STACK_LIMIT;
      MAX_MANA = DEF_MAX_MANA;
      MAX_FLOATING_MANA = DEF_MAX_FLOATING_MANA;
      MANA_START = DEF_MANA_START;
      FLOATING_MANA_START = DEF_FLOATING_MANA_START;
      INITIAL_HAND_SIZE = DEF_INITIAL_HAND_SIZE;
      MAX_ROUNDS = DEF_MAX_ROUNDS;
      INVALID_ACTIONS_LIMIT = DEF_INVALID_ACTIONS_LIMIT;
      ENLIGHTENMENT_THRESHOLD = DEF_ENLIGHTENMENT_THRESHOLD;
   }
};

#endif  // LORAINE_CONFIG_H
