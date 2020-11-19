
#ifndef LORAINE_TARGET_H
#define LORAINE_TARGET_H

#include <utility>

#include "cards/context_card.h"
#include "rulesets.h"
#include "types.h"

class Card;

class BaseTarget {
  public:
   virtual ~BaseTarget() = 0;
   BaseTarget(const BaseTarget& other) = default;
   BaseTarget(BaseTarget&&) = default;
   BaseTarget& operator=(const BaseTarget& other) = default;
   BaseTarget(bool is_nexus, Player player) : m_is_nexus(is_nexus), m_player(player) {}
   [[nodiscard]] auto is_nexus() const { return m_is_nexus; }
   [[nodiscard]] auto get_player() const { return m_player; }

  private:
   bool m_is_nexus;
   Player m_player;
};

class NexusTarget: public BaseTarget {
  public:
   explicit NexusTarget(Player player) : BaseTarget(true, player) {}
};

class CardTarget: public BaseTarget {
  public:
   CardTarget(Player player, Location loc, size_t index, const sptr< Card >& card)
       : BaseTarget(false, player), m_contextcard(loc, index, card)
   {
   }
   [[nodiscard]] auto get_contextcard() const { return m_contextcard; }

  private:
   ContextCard m_contextcard;
};

sptr< NexusTarget > to_nexus_target(const sptr< BaseTarget >& target);
sptr< CardTarget > to_card_target(const sptr< BaseTarget >& target);

#endif  // LORAINE_TARGET_H
