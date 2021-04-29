
#ifndef LORAINE_ACTION_SYTEM_H
#define LORAINE_ACTION_SYTEM_H

#include <set>
#include <utility>

#include "loraine/core/action.h"
#include "loraine/utils/types.h"
#include "system.hpp"

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
class ActionSystemBase {
  public:
   enum Phase { DEFAULT = 0, COMBAT, MULLIGAN, REPLACING, TARGET };
   ActionSystemBase(Phase phase, std::set< actions::ActionID > accepted_actions)
       : m_phase(phase), m_accepted_actions(std::move(accepted_actions))
   {
   }
   virtual ~ActionSystemBase() = default;
   ActionSystemBase(const ActionSystemBase& other) = default;
   ActionSystemBase(ActionSystemBase&& other) = default;
   ActionSystemBase& operator=(ActionSystemBase&& other) = delete;
   ActionSystemBase& operator=(const ActionSystemBase& other) = delete;

   [[nodiscard]] virtual actions::Action request_action(const GameState& state) const;
   virtual bool invoke(GameState& state, actions::Action& action);
   [[nodiscard]] virtual bool is_valid(GameState& state, const actions::Action& action) const = 0;
   [[nodiscard]] virtual std::vector< actions::Action > valid_actions(
      const GameState& state) const = 0;

   auto& accepted_actions() { return m_accepted_actions; }
   [[nodiscard]] auto& accepted_actions() const { return m_accepted_actions; }
   virtual ActionSystemBase* clone() = 0;
   [[nodiscard]] auto phase() const { return m_phase; }

  private:
   const Phase m_phase;
   const std::set< actions::ActionID > m_accepted_actions;
   std::vector< actions::Action > m_action_stack;
};

template < typename Derived, actions::ActionID... AcceptedActions >
class ActionPhase: public ActionSystemBase {
  public:
   using base = ActionSystemBase;
   using base::base;

   ActionPhase() : base(Derived::phase_id, std::set< actions::ActionID >{AcceptedActions...}) {}
   template < typename... Args >
   ActionPhase(Args... args)
       : base(
          Derived::phase_label,
          std::forward< Args >(args)...,
          std::set< actions::ActionID >{AcceptedActions...})
   {
   }
   inline ActionSystemBase* clone() override { return new Derived(static_cast< Derived& >(*this)); }
};

class DefaultActionPhase:
    public ActionPhase<
       DefaultActionPhase,
       actions::ActionID::ACCEPT,
       actions::ActionID::PLAY_FIELDCARD,
       actions::ActionID::DRAG_ENEMY,
       actions::ActionID::PLACE_UNIT,
       actions::ActionID::PLACE_SPELL > {
  public:
   using base = ActionPhase<
      DefaultActionPhase,
      actions::ActionID::ACCEPT,
      actions::ActionID::PLAY_FIELDCARD,
      actions::ActionID::DRAG_ENEMY,
      actions::ActionID::PLACE_UNIT,
      actions::ActionID::PLACE_SPELL >;
   using base::base;

   constexpr static Phase phase_label = Phase::DEFAULT;
   bool is_valid(GameState& state, const actions::Action& action) const override;

   std::vector< actions::Action > valid_actions(const GameState& action) const override;
};

class CombatActionPhase:
    public ActionPhase<
       CombatActionPhase,
       actions::ActionID::ACCEPT,
       actions::ActionID::PLACE_SPELL > {
  public:
   using base = ActionPhase<
      CombatActionPhase,
      actions::ActionID::ACCEPT,
      actions::ActionID::PLACE_SPELL >;
   using base::base;

   constexpr static Phase phase_label = Phase::COMBAT;
   bool is_valid(GameState& state, const actions::Action& action) const override;

   std::vector< actions::Action > valid_actions(const GameState& action) const override;
};

class TargetActionPhase:
    public ActionPhase<
       TargetActionPhase,
       actions::ActionID::ACCEPT,
       actions::ActionID::TARGETING > {
  public:
   using base = ActionPhase<
      TargetActionPhase,
      actions::ActionID::ACCEPT,
      actions::ActionID::TARGETING >;
   using base::base;

   constexpr static Phase phase_label = Phase::TARGET;
   [[nodiscard]] actions::Action request_action(const GameState& state) const override;
   bool is_valid(GameState& state, const actions::Action& action) const override;

   std::vector< actions::Action > valid_actions(const GameState& action) const override;
};

class ReplacingActionPhase:
    public ActionPhase<
       ReplacingActionPhase,
       actions::ActionID::CANCEL,
       actions::ActionID::REPLACE_FIELDCARD > {
  public:
   using base = ActionPhase<
      ReplacingActionPhase,
      actions::ActionID::CANCEL,
      actions::ActionID::REPLACE_FIELDCARD >;
   using base::base;

   constexpr static Phase phase_label = Phase::REPLACING;
   [[nodiscard]] actions::Action request_action(const GameState& state) const override;
   bool is_valid(GameState& state, const actions::Action& action) const override;

   std::vector< actions::Action > valid_actions(const GameState& action) const override;
};

class MulliganActionPhase: public ActionPhase< MulliganActionPhase, actions::ActionID::MULLIGAN > {
  public:
   using base = ActionPhase< MulliganActionPhase, actions::ActionID::MULLIGAN >;
   using base::base;

   constexpr static Phase phase_label = Phase::MULLIGAN;
   bool is_valid(GameState& state, const actions::Action& action) const override;
   std::vector< actions::Action > valid_actions(const GameState& action) const override;
};

namespace helpers {
template < ActionSystemBase::Phase p >
struct phase_to_type;

template <>
struct phase_to_type< ActionSystemBase::Phase::DEFAULT > {
   constexpr static auto phase = ActionSystemBase::Phase::DEFAULT;
   using type = DefaultActionPhase;
};
template <>
struct phase_to_type< ActionSystemBase::Phase::COMBAT > {
   constexpr static auto phase = ActionSystemBase::Phase::COMBAT;
   using type = CombatActionPhase;
};
template <>
struct phase_to_type< ActionSystemBase::Phase::REPLACING > {
   constexpr static auto phase = ActionSystemBase::Phase::REPLACING;
   using type = ReplacingActionPhase;
};
template <>
struct phase_to_type< ActionSystemBase::Phase::TARGET > {
   constexpr static auto phase = ActionSystemBase::Phase::TARGET;
   using type = TargetActionPhase;
};

template < ActionSystemBase::Phase p >
using phase_to_type_t = typename phase_to_type< p >::type;

}  // namespace helpers

#endif  // LORAINE_ACTION_SYTEM_H
