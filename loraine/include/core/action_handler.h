
#ifndef LORAINE_ACTION_HANDLER_H
#define LORAINE_ACTION_HANDLER_H

#include <utility>

#include "action.h"
#include "utils/types.h"

class Logic;
class EffectBase;

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
class ActionHandlerBase {
  protected:
  public:
   virtual ActionHandlerBase* clone() = 0;
   virtual bool handle(const sptr< Action >& action) = 0;
   virtual bool is_valid(const sptr< Action >& action) = 0;

   explicit ActionHandlerBase(Logic* logic, std::vector< ActionLabel >  accepted_actions)
       : m_logic(logic), m_accepted_actions(std::move(accepted_actions))
   {
   }
   virtual ~ActionHandlerBase() = default;
   void logic(Logic* state) { m_logic = state; }
   auto logic() { return m_logic; }
   auto* accepted_actions() { return &m_accepted_actions;}
   [[nodiscard]] auto* accepted_actions() const { return &m_accepted_actions;}

  protected:
   // this handling is shared by multiple action handlers, thus should be defined for all
   bool _handle(const sptr< PlaceSpellAction >& action);
   bool _handle(const sptr< PassAction >& action);

  private:
   Logic* m_logic;
   const std::vector< ActionLabel > m_accepted_actions;
};

template < ActionLabel... AcceptedActions >
class ActionHandler: public ActionHandlerBase {
  public:
   template < typename... Args >
   ActionHandler(Args... args)
       : ActionHandlerBase(
          std::forward< Args >(args)...,
          std::vector< ActionLabel >{AcceptedActions...})
   {
   }
};

class DefaultModeHandler:
    public ActionHandler<
       ActionLabel::PASS,
       ActionLabel::ACCEPT,
       ActionLabel::PLAY,
       ActionLabel::DRAG_ENEMY,
       ActionLabel::PLACE_UNIT,
       ActionLabel::PLACE_SPELL > {
  public:
   explicit DefaultModeHandler(Logic* logic) : ActionHandler(logic){};
   inline ActionHandlerBase* clone() override { return new DefaultModeHandler(logic()); }
   bool handle(const sptr< Action >& action) override;
   bool is_valid(const sptr< Action >& action) override;

  private:
   bool _handle(const sptr< PlayAction >& action);
   bool _handle(const sptr< PlaceUnitAction >& action);
   bool _handle(const sptr< DragEnemyAction >& action);
   bool _handle(const sptr< AcceptAction >& action);
};

class CombatModeHandler: public ActionHandler< ActionLabel::ACCEPT, ActionLabel::PLACE_SPELL > {
  public:
   explicit CombatModeHandler(Logic* logic) : ActionHandler(logic){};
   inline ActionHandlerBase* clone() override { return new CombatModeHandler(logic()); }
   bool handle(const sptr< Action >& action) override;
   bool is_valid(const sptr< Action >& action) override;
};

class TargetModeHandler: public ActionHandler< ActionLabel::TARGETING > {
  public:
   explicit TargetModeHandler(Logic* logic, ActionHandlerBase* prev_handler)
       : ActionHandler(logic), m_prev_handler(prev_handler){};
   ~TargetModeHandler() override { delete m_prev_handler; }
   inline ActionHandlerBase* clone() override
   {
      if(dynamic_cast< TargetModeHandler* >(m_prev_handler) == nullptr) {
         // if the previous handler was not of target mode then clone another such handler
         return new TargetModeHandler(logic(), m_prev_handler->clone());
      }
      // otherwise just forward it
      return new TargetModeHandler(logic(), m_prev_handler);
   }
   bool handle(const sptr< Action >& action) override;
   bool is_valid(const sptr< Action >& action) override;

  private:
   ActionHandlerBase* m_prev_handler;
};

class MulliganModeHandler: public ActionHandler< ActionLabel::MULLIGAN > {
  public:
   explicit MulliganModeHandler(Logic* logic) : ActionHandler(logic){};
   inline ActionHandlerBase* clone() override { return new MulliganModeHandler(logic()); }
   bool handle(const sptr< Action >& action) override;
   bool is_valid(const sptr< Action >& action) override;
};

#endif  // LORAINE_ACTION_HANDLER_H
