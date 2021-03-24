
#ifndef LORAINE_TRIBUTE_H
#define LORAINE_TRIBUTE_H

#include "utils/types.h"

class State;
class Card;

/**
 * The tribute class executes a non-mana cost or condition a team has to pay/
 */
class PlayCondition : public Cloneable<PlayCondition>{
  public:
   PlayCondition() = default;
   PlayCondition(const PlayCondition& other) = default;
   PlayCondition(PlayCondition&& other) = delete;
   PlayCondition& operator=(const PlayCondition& other) = default;
   PlayCondition& operator=(PlayCondition&& other) = delete;
   ~PlayCondition() override = default;

   virtual void pay(State& state, Card& card) {}  // the default tribute is no tribute at all
   virtual bool check(State& state, Card& card) {return true;}  // per default no play condition

};

#endif  // LORAINE_TRIBUTE_H
