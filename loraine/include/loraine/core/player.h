
#ifndef LORAINE_PLAYER_H
#define LORAINE_PLAYER_H

#include "deck.h"
#include "hand.h"
#include "loraine/controller.h"
#include "nexus.h"

struct ManaResource {
   void incr_gems(size_t amount = 1) { gems += amount; }
   void refill() { mana = gems; }
   inline long spend(size_t amount)
   {
      if(mana > amount) {
         mana -= amount;
         return 0;
      }
      mana = 0;
      return -long(amount - mana);
   }

  private:
   size_t gems = 0;
   size_t mana = 0;
};




class Player {
  public:
   struct Flags {
      bool attack_token = false;
      bool scout_token = false;
      bool plunder_token = false;
      bool is_daybreak = true;
      bool is_nightfall = false;
      bool enlightened = false;
      bool has_played = false;
      bool pass = false;
   };



   Player(
      Team team,
      Nexus nexus,
      Deck deck,
      sptr< Controller > controller,
      Mana mana = {0, 0, 0},
      Flags flags = {false, false, false, false, false, false, false, false});
   ~Player() = default;
   Player(const Player& other);
   Player(Player&& other) = default;
   Player& operator=(const Player& other) = delete;
   Player& operator=(Player&& other) = delete;

   inline auto& nexus() { return m_nexus; }
   [[nodiscard]] inline auto& nexus() const { return m_nexus; }
   [[nodiscard]] inline auto team() const { return m_team; }

   [[nodiscard]] inline auto controller() { return m_controller; }
   [[nodiscard]] inline auto controller() const { return m_controller; }

   inline auto& flags() { return m_flags; }
   [[nodiscard]] inline auto& flags() const { return m_flags; }

   inline void hand(HandType hand) { m_hand = std::move(hand); }
   [[nodiscard]] inline auto& hand() const { return m_hand; }
   [[nodiscard]] inline auto& hand() { return m_hand; }

   inline void deck(Deck deck) { m_deck = std::move(deck); }
   [[nodiscard]] inline auto& deck() const { return m_deck; }
   [[nodiscard]] inline auto& deck() { return m_deck; }

   inline void mana(Mana mana) { m_mana = mana; }
   [[nodiscard]] inline auto& mana() { return m_mana; }
   [[nodiscard]] inline auto& mana() const { return m_mana; }

   [[nodiscard]] inline auto& playyard() { return m_playyard; }
   [[nodiscard]] inline auto& playyard() const { return m_playyard; }
   [[nodiscard]] inline auto& graveyard() { return m_graveyard; }
   [[nodiscard]] inline auto& graveyard() const { return m_graveyard; }
   [[nodiscard]] inline auto& spellyard() { return m_spellyard; }
   [[nodiscard]] inline auto& spellyard() const { return m_spellyard; }
   [[nodiscard]] inline auto& tossed_cards() { return m_tossed_cards; }
   [[nodiscard]] inline auto& tossed_cards() const { return m_tossed_cards; }

  private:
   Team m_team;
   Nexus m_nexus;
   sptr< Controller > m_controller;
   HandType m_hand;
   Deck m_deck;
   Mana m_mana;
   Flags m_flags;
   std::map< size_t, std::vector< sptr< Card > > > m_playyard = {};  // all the cards played
   std::map< size_t, std::vector< sptr< FieldCard > > > m_graveyard = {};
   std::map< size_t, std::vector< sptr< Spell > > > m_spellyard = {};
   std::vector< sptr< Card > > m_tossed_cards = {};
};

#endif  // LORAINE_PLAYER_H
