
#ifndef LORAINE_ACTION_HANDLER_H
#define LORAINE_ACTION_HANDLER_H

#include "utils/types.h"

class Action;
class Logic;


/*
 * Class handling the logic of the LoR State. It has different modes for handling
 * a given Action object using the 'State' pattern (unrelated to class 'State').
 * The context for the Logic is the State object on which to act.
 *
 * In general the base logic provides the game logic of LoR, with the subclasses
 * implementing different methods of dealing with actions.
 *
 * The specific subclasses are to implement 6 different game modes:
 *    1. Active-mode: Handle actions for
 *          i)    moving units onto the battlefield
 *          ii)   placing spells on the stack
 *          iii)  playing a field card or the placed spells or a burst floating
 *          iv)   passing
 *    2. Attack-mode: Handle actions for
 *          i)    declaring an attack with the moved units
 *          ii)   dragging opponent units onto the battlefield
 *          iii)  moving (fast, burst) spells onto the stack
 *          iv)   playing the placed spells or a burst floating
 *    3. Block-mode: Handle actions for
 *          i)    moving units onto the attacking positions on the battlefield
 *          ii)   moving (fast, burst) spells onto the stack
 *          iii)  playing a burst floating
 *    4. Combat-mode: Handle actions for
 *          i)    placing a (fast, burst) floating on the stack
 *          ii)   playing the placed spells or a burst floating
 *          ii)   accepting
 *    5. Passive-mode: Handle actions for
 *          i)    placing spells on the stack
 *          ii)   playing a field card or floating
 *          iii)  passing
 *    6. Targeting-mode: Handle actions for
 *          i)    targeting a specific amount of cards
 *    7. Mulligan-mode: Handle actions for
 *          i)    mulligan the starting hand cards
 *
 */
class ActionHandler {
  public:

   virtual void handle(const sptr< Action >& action) = 0;
   virtual bool is_valid(const sptr< Action >& action) = 0;

   explicit ActionHandler(Logic* logic = nullptr): m_logic(logic) {}

   void logic(Logic* state) { m_logic = state;}
   auto logic() {return m_logic;}
   [[nodiscard]] auto state() const {return m_logic;}

  private:
   Logic* m_logic;
};

class DefaultModeHandler: public ActionHandler {
   void handle(const sptr< Action >& action) override;
   bool is_valid(const sptr< Action >& action) override;
};
class CombatModeHandler: public ActionHandler {
   void handle(const sptr< Action >& action) override;
   bool is_valid(const sptr< Action >& action) override;
};
class TargetModeHandler: public ActionHandler {
   void handle(const sptr< Action >& action) override;
   bool is_valid(const sptr< Action >& action) override;
};
class MulliganModeHandler: public ActionHandler {
   void handle(const sptr< Action >& action) override;
   bool is_valid(const sptr< Action >& action) override;
};




#endif  // LORAINE_ACTION_HANDLER_H