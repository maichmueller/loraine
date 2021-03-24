
#ifndef LORAINE_ACTION_HANDLER_H
#define LORAINE_ACTION_HANDLER_H

#include <utility>

#include "action.h"
#include "utils/types.h"

class Logic;
class EffectBase;
class State;

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
 *    2. Combat-mode: Handle actions for
 *          i)    placing a (fast, burst) floating on the stack
 *          ii)   playing the placed spells or a burst floating
 *          ii)   accepting
 *    3. Targeting-mode: Handle actions for
 *          i)    targeting a specific amount of cards
 *    4. Mulligan-mode: Handle actions for
 *          i)    mulligan the starting hand cards
 *
 */
class ActionHandlerBase {
  public:
   enum Label { DEFAULT = 0, COMBAT, MULLIGAN, TARGET };
   explicit ActionHandlerBase(
      Label label,
      Logic* logic,
      std::vector< actions::ActionLabel > accepted_actions)
       : m_label(label), m_logic(logic), m_accepted_actions(std::move(accepted_actions))
   {
   }
   explicit ActionHandlerBase(Label label, std::vector< actions::ActionLabel > accepted_actions)
       : m_label(label), m_logic(nullptr), m_accepted_actions(std::move(accepted_actions))
   {
   }
   virtual ~ActionHandlerBase() = default;
   [[nodiscard]] virtual actions::Action request_action(const State& state) const;
   [[nodiscard]] virtual bool is_valid(const actions::Action& action) const = 0;
   [[nodiscard]] virtual std::vector< actions::Action > valid_actions(const State& state) const = 0;
   void logic(Logic* logic) { m_logic = logic; }

   auto logic() { return m_logic; }
   auto* accepted_actions() { return &m_accepted_actions; }
   [[nodiscard]] auto label() const { return m_label; }
   [[nodiscard]] auto* accepted_actions() const { return &m_accepted_actions; }

  private:
   const Label m_label;
   Logic* m_logic;
   const std::vector< actions::ActionLabel > m_accepted_actions;
};

template < typename Derived, actions::ActionLabel... AcceptedActions >
class ActionHandler: public ActionHandlerBase {
  public:
   using base = ActionHandler< Derived, AcceptedActions... >;
   template < typename... Args >
   ActionHandler(Args... args)
       : ActionHandlerBase(
          std::forward< Args >(args)...,
          std::vector< actions::ActionLabel >{AcceptedActions...})
   {
   }
};

class DefaultModeHandler:
    public ActionHandler<
       DefaultModeHandler,
       actions::ActionLabel::PASS,
       actions::ActionLabel::ACCEPT,
       actions::ActionLabel::PLAY,
       actions::ActionLabel::DRAG_ENEMY,
       actions::ActionLabel::PLACE_UNIT,
       actions::ActionLabel::PLACE_SPELL > {
  public:
   using base::base;
   bool is_valid(const actions::Action& action) const override;
   std::vector< actions::Action > valid_actions(const State& action) const override;
};

class CombatModeHandler:
    public ActionHandler<
       CombatModeHandler,
       actions::ActionLabel::ACCEPT,
       actions::ActionLabel::PLACE_SPELL > {
  public:
   using base::base;
   bool is_valid(const actions::Action& action) const override;
   std::vector< actions::Action > valid_actions(const State& action) const override;
};

class TargetModeHandler:
    public ActionHandler< TargetModeHandler, actions::ActionLabel::TARGETING > {
  public:
   using base::base;
   [[nodiscard]] actions::Action request_action(const State& state) const override;
   bool is_valid(const actions::Action& action) const override;
   std::vector< actions::Action > valid_actions(const State& action) const override;
};

class MulliganModeHandler:
    public ActionHandler< MulliganModeHandler, actions::ActionLabel::MULLIGAN > {
  public:
   using base::base;

   bool is_valid(const actions::Action& action) const override;
   std::vector< actions::Action > valid_actions(const State& action) const override;
};

#endif  // LORAINE_ACTION_HANDLER_H
