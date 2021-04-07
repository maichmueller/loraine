
#ifndef LORAINE_ACTION_INVOKER_H
#define LORAINE_ACTION_INVOKER_H

#include <utility>

#include "action.h"
#include "utils/types.h"

class Logic;
class GameState;

/*
 * Class handling the logic of the LoR GameState. It has different modes for handling
 * a given Action object using the 'GameState' pattern (unrelated to class 'GameState').
 * The context for the Logic is the GameState object on which to act.
 *
 * In general the base logic provides the game logic of LoR, with the subclasses
 * implementing different methods of dealing with actions.
 *
 * The specific subclasses are to implement 6 different game modes:
 *    1. Active-mode: Handle actions for
 *          i)    moving units onto the battlefield
 *          ii)   placing spells on the stack
 *          iii)  playing a field spell or the placed spells or a burst floating
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
class ActionInvokerBase {
  public:
   enum Label { DEFAULT = 0, COMBAT, MULLIGAN, REPLACING, TARGET };
   explicit ActionInvokerBase(
      Label label,
      Logic* logic,
      std::set< actions::ActionLabel > accepted_actions)
       : m_label(label), m_logic(logic), m_accepted_actions(std::move(accepted_actions))
   {
   }
   explicit ActionInvokerBase(Label label, std::set< actions::ActionLabel > accepted_actions)
       : m_label(label), m_logic(nullptr), m_accepted_actions(std::move(accepted_actions))
   {
   }
   virtual ~ActionInvokerBase() = default;
   ActionInvokerBase(const ActionInvokerBase& other) = default;
   ActionInvokerBase(ActionInvokerBase&& other) = default;
   ActionInvokerBase& operator=(ActionInvokerBase&& other) = delete;
   ActionInvokerBase& operator=(const ActionInvokerBase& other) = delete;

   [[nodiscard]] virtual actions::Action request_action(const GameState& state) const;
   virtual bool invoke(actions::Action& action);
   [[nodiscard]] virtual bool is_valid(const actions::Action& action) const = 0;
   [[nodiscard]] virtual std::vector< actions::Action > valid_actions(
      const GameState& state) const = 0;

   void logic(Logic* logic) { m_logic = logic; }
   auto logic() { return m_logic; }

   auto* accepted_actions() { return &m_accepted_actions; }
   [[nodiscard]] auto* accepted_actions() const { return &m_accepted_actions; }
   virtual ActionInvokerBase* clone() = 0;
   [[nodiscard]] auto label() const { return m_label; }

  private:
   const Label m_label;
   Logic* m_logic;
   const std::set< actions::ActionLabel > m_accepted_actions;
};

template < typename Derived, actions::ActionLabel... AcceptedActions >
class ActionInvoker: public ActionInvokerBase {
  public:
   using base = ActionInvokerBase;
   using base::base;

   ActionInvoker(Logic* logic = nullptr)
       : base(Derived::invoker_label, logic, std::set< actions::ActionLabel >{AcceptedActions...})
   {
   }
   template < typename... Args >
   ActionInvoker(Args... args)
       : base(
          Derived::invoker_label,
          std::forward< Args >(args)...,
          std::set< actions::ActionLabel >{AcceptedActions...})
   {
   }
   inline ActionInvokerBase* clone() override
   {
      return new Derived(static_cast< Derived& >(*this));
   }
};

class DefaultModeInvoker:
    public ActionInvoker<
       DefaultModeInvoker,
       actions::ActionLabel::ACCEPT,
       actions::ActionLabel::PLAY_FIELDCARD,
       actions::ActionLabel::DRAG_ENEMY,
       actions::ActionLabel::PLACE_UNIT,
       actions::ActionLabel::PLACE_SPELL > {
  public:
   using base = ActionInvoker<
      DefaultModeInvoker,
      actions::ActionLabel::ACCEPT,
      actions::ActionLabel::PLAY_FIELDCARD,
      actions::ActionLabel::DRAG_ENEMY,
      actions::ActionLabel::PLACE_UNIT,
      actions::ActionLabel::PLACE_SPELL >;
   using base::base;

   constexpr static Label invoker_label = Label::DEFAULT;
   bool is_valid(const actions::Action& action) const override;

   std::vector< actions::Action > valid_actions(const GameState& action) const override;
};

class CombatModeInvoker:
    public ActionInvoker<
       CombatModeInvoker,
       actions::ActionLabel::ACCEPT,
       actions::ActionLabel::PLACE_SPELL > {
  public:
   using base = ActionInvoker<
      CombatModeInvoker,
      actions::ActionLabel::ACCEPT,
      actions::ActionLabel::PLACE_SPELL >;
   using base::base;

   constexpr static Label invoker_label = Label::COMBAT;
   bool is_valid(const actions::Action& action) const override;

   std::vector< actions::Action > valid_actions(const GameState& action) const override;
};

class TargetModeInvoker:
    public ActionInvoker<
       TargetModeInvoker,
       actions::ActionLabel::ACCEPT,
       actions::ActionLabel::TARGETING > {
  public:
   using base = ActionInvoker<
      TargetModeInvoker,
      actions::ActionLabel::ACCEPT,
      actions::ActionLabel::TARGETING >;
   using base::base;

   constexpr static Label invoker_label = Label::TARGET;
   [[nodiscard]] actions::Action request_action(const GameState& state) const override;
   bool is_valid(const actions::Action& action) const override;

   std::vector< actions::Action > valid_actions(const GameState& action) const override;
};

class ReplacingModeInvoker:
    public ActionInvoker<
       ReplacingModeInvoker,
       actions::ActionLabel::CANCEL,
       actions::ActionLabel::REPLACE_FIELDCARD > {
  public:
   using base = ActionInvoker<
      ReplacingModeInvoker,
      actions::ActionLabel::CANCEL,
      actions::ActionLabel::REPLACE_FIELDCARD >;
   using base::base;

   constexpr static Label invoker_label = Label::REPLACING;
   [[nodiscard]] actions::Action request_action(const GameState& state) const override;
   bool is_valid(const actions::Action& action) const override;

   std::vector< actions::Action > valid_actions(const GameState& action) const override;
};

class MulliganModeInvoker:
    public ActionInvoker< MulliganModeInvoker, actions::ActionLabel::MULLIGAN > {
  public:
   using base = ActionInvoker< MulliganModeInvoker, actions::ActionLabel::MULLIGAN >;
   using base::base;

   constexpr static Label invoker_label = Label::MULLIGAN;
   bool is_valid(const actions::Action& action) const override;
   std::vector< actions::Action > valid_actions(const GameState& action) const override;
};

#endif  // LORAINE_ACTION_INVOKER_H
