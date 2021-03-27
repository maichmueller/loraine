
#ifndef LORAINE_TOLL_H
#define LORAINE_TOLL_H

#include "utils/types.h"

class State;
class Card;

/**
 * The Toll class executes a non-mana cost or condition a team has to pay/
 */
class Toll: public Cloneable< Toll > {
  public:
   Toll() = default;
   Toll(const Toll& other) = default;
   Toll(Toll&& other) = default;
   Toll& operator=(const Toll& other) = default;
   Toll& operator=(Toll&& other) = default;
   ~Toll() override = default;

   virtual void pay(State& state, Card& card) {}  // the default tribute is no tribute at all
   virtual bool check(State& state, Card& card) { return true; }  // per default no play condition
};

#endif  // LORAINE_TOLL_H
