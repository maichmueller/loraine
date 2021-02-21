
#ifndef LORAINE_TRIBUTE_H
#define LORAINE_TRIBUTE_H

class State;
class Card;

/**
 * The tribute class executes a non-mana cost or condition a player has to pay/  
 */
class PlayCondition {
   virtual void pay(State& state, Card& card) {}  // the default tribute is no tribute at all
   virtual bool check(State& state, Card& card) {return true;}  // per default no play condition
};

#endif  // LORAINE_TRIBUTE_H
