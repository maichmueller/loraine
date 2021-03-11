
#include "core/player.h"

#include <utility>
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
