
#ifndef LORAINE_TRAP_H
#define LORAINE_TRAP_H

#include "cardbase.h"

class TrapCard: public Card {
   struct MutableTrapState {
      std::vector< sptr< Card > > trapped_cards;
   };

   [[nodiscard]] bool is_trap() const override { return true; }

   TrapCard(ConstState const_state, MutableState mut_state, MutableTrapState mut_trap_state)
       : Card(std::move(const_state), std::move(mut_state)),
         m_trap_mutables(std::move(mut_trap_state))
   {
   }

  private:
   MutableTrapState m_trap_mutables;
};

#endif  // LORAINE_TRAP_H
