
#ifndef LORAINE_TRAP_H
#define LORAINE_TRAP_H

#include "cardbase.h"

class TrapCard: public Cloneable<abstract_method<TrapCard>, inherit_constructors<Card>> {
  public:
   struct MutableTrapState {
      std::vector< sptr< Card > > trapped_cards;
   };

   TrapCard(ConstState const_state, MutableState mut_state, MutableTrapState mut_trap_state)
       : Cloneable(std::move(const_state), std::move(mut_state)),
         m_trap_mutables(std::move(mut_trap_state))
   {
   }

  private:
   MutableTrapState m_trap_mutables;
};

#endif  // LORAINE_TRAP_H
