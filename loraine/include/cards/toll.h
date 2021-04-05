
#ifndef LORAINE_TOLL_H
#define LORAINE_TOLL_H

#include "utils/types.h"

class GameState;
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

   virtual void pay(GameState& state, Card& card) {}  // the default tribute is no tribute at all
   virtual bool check(GameState& state, Card& card) { return true; }  // per default no play_event_triggers condition
};

#endif  // LORAINE_TOLL_H
