
#ifndef LORAINE_PLAYER_H
#define LORAINE_PLAYER_H

#include "controller.h"
#include "deck.h"

class Player {
  public:
   struct Flags {
      bool attack_token = false;
      bool scout_token = false;
      bool plunder_token = false;
      bool pass = false;
   };
   struct Mana {
      size_t gems = 0;
      size_t common = 0;  // common mana to play fieldcards with
      size_t floating = 0;  // mana exclusively for spells
   };
   using HandType = std::vector< sptr< Card > >;
   using DeckType = std::vector< sptr< Card > >;

   inline auto* nexus() { return &m_nexus; }
   [[nodiscard]] inline auto* nexus() const { return &m_nexus; }
   [[nodiscard]] inline auto team() const { return m_team; }

   [[nodiscard]] inline auto* controller() { return &m_controller; }
   [[nodiscard]] inline auto* controller() const { return &m_controller; }

   inline auto* flags() { return &m_flags; }
   [[nodiscard]] inline auto* flags() const { return &m_flags; }

   inline void hand(HandType hand) { m_hand = std::move(hand); }
   [[nodiscard]] inline auto* hand() const { return &m_hand; }
   [[nodiscard]] inline auto* hand() { return &m_hand; }

   inline void deck(DeckType deck) { m_deck_cont = std::move(deck); }
   [[nodiscard]] inline auto* deck() const { return &m_deck_cont; }
   [[nodiscard]] inline auto* deck() { return &m_deck_cont; }

   inline void mana(Mana mana) { m_mana = mana; }
   [[nodiscard]] inline auto* mana() { return &m_mana; }
   [[nodiscard]] inline auto* mana() const { return &m_mana; }

   [[nodiscard]] inline auto* history() { return &m_history; }
   [[nodiscard]] inline auto* history() const { return &m_history; }
   [[nodiscard]] inline auto* graveyard() { return &m_graveyard; }
   [[nodiscard]] inline auto* graveyard() const { return &m_graveyard; }
   [[nodiscard]] inline auto* tossed_cards() { return &m_tossed_cards; }
   [[nodiscard]] inline auto* tossed_cards() const { return &m_tossed_cards; }

  private:
   Nexus m_nexus;
   sptr< Controller > m_controller;
   HandType m_hand;
   DeckType m_deck_cont;
   Team m_team;
   Mana m_mana;
   Flags m_flags;
   std::map< size_t, std::vector< sptr< Card > > > m_graveyard = {};
   std::vector< sptr< Card > > m_tossed_cards = {};
   std::map< size_t, std::vector< sptr< Action > > > m_history = {};
};

#endif  // LORAINE_PLAYER_H
