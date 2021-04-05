
#include "core/player.h"

#include <utility>

#include "cards/card.h"
Player::Player(
   Team team,
   Nexus nexus,
   Deck deck,
   sptr< Controller > controller,
   Player::Mana mana,
   Player::Flags flags)
    : m_team(team),
      m_nexus(std::move(nexus)),
      m_controller(std::move(controller)),
      m_hand(),
      m_deck(std::move(deck)),
      m_mana(mana),
      m_flags(flags)
{
}
Player::Player(const Player& other)
    : m_team(other.m_team),
      m_nexus(other.m_nexus),
      m_controller(other.m_controller),  // the controller is not copied, since we assume the same
                                         // BOT or human should control this copy
      m_hand(),  // hand is only a vector and thus needs to be coopied manually
      m_deck(other.m_deck),  // deck has copy constructor which clones the spell pointers
      m_mana(other.m_mana),
      m_flags(other.m_flags)
{
   m_hand.reserve(other.m_hand.size());
   for(const auto& card : other.m_hand) {
      m_hand.emplace_back(card->clone());
   }
}
