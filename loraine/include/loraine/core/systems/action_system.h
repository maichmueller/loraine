
#ifndef LORAINE_ACTION_SYSTEM_H
#define LORAINE_ACTION_SYSTEM_H

#include <set>
#include <utility>

#include "loraine/core/action.h"
#include "loraine/utils/types.h"
#include "system.hpp"

class GameState;

class ActionSystem;

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
class ActionPhaseBase {
  public:
   ActionPhaseBase(
      ActionSystem* act_system,
      actions::Phase phase,
      std::set< actions::ActionID > accepted_actions)
       : m_action_system(act_system),
         m_phase(phase),
         m_accepted_actions(std::move(accepted_actions))
   {
   }
   virtual ~ActionPhaseBase() = default;
   ActionPhaseBase(const ActionPhaseBase& other) = default;
   ActionPhaseBase(ActionPhaseBase&& other) = default;
   ActionPhaseBase& operator=(ActionPhaseBase&& other) = delete;
   ActionPhaseBase& operator=(const ActionPhaseBase& other) = delete;

   [[nodiscard]] virtual actions::Action request_action(const GameState& state) const;
   virtual bool invoke(actions::Action& action) = 0;
   [[nodiscard]] virtual bool is_valid(GameState& state, const actions::Action& action) const = 0;
   [[nodiscard]] virtual std::vector< actions::Action > valid_actions(
      const GameState& state) const = 0;

   auto& accepted_actions() { return m_accepted_actions; }
   [[nodiscard]] auto& accepted_actions() const { return m_accepted_actions; }
   virtual ActionPhaseBase* clone() = 0;
   [[nodiscard]] auto phase() const { return m_phase; }

  private:
   ActionSystem* m_action_system;
   actions::Phase m_phase;
   std::set< actions::ActionID > m_accepted_actions;
};

template < typename Derived, actions::ActionID... AcceptedActions >
class ActionPhase: public ActionPhaseBase {
  public:
   using base = ActionPhaseBase;
   using base::base;

   ActionPhase(ActionSystem* system)
       : base(system, Derived::phase_label, std::set< actions::ActionID >{AcceptedActions...})
   {
   }
   template < typename... Args >
   ActionPhase(ActionSystem* system, Args... args)
       : base(
          system,
          Derived::phase_label,
          std::forward< Args >(args)...,
          std::set< actions::ActionID >{AcceptedActions...})
   {
   }
   inline ActionPhaseBase* clone() override { return new Derived(static_cast< Derived& >(*this)); }
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

   constexpr static actions::Phase phase_label = actions::Phase::COMBAT;
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

   constexpr static actions::Phase phase_label = actions::Phase::TARGET;
   [[nodiscard]] actions::Action request_action(const GameState& state) const override;
   bool is_valid(GameState& state, const actions::Action& action) const override;

   std::vector< actions::Action > valid_actions(const GameState& action) const override;
};

class ReplacingActionPhase:
    public ActionPhase<
       ReplacingActionPhase,
       actions::ActionID::CANCEL,
       actions::ActionID::REPLACE > {
  public:
   using base = ActionPhase<
      ReplacingActionPhase,
      actions::ActionID::CANCEL,
      actions::ActionID::REPLACE >;
   using base::base;

   constexpr static actions::Phase phase_label = actions::Phase::REPLACING;
   [[nodiscard]] actions::Action request_action(const GameState& state) const override;
   bool is_valid(GameState& state, const actions::Action& action) const override;

   std::vector< actions::Action > valid_actions(const GameState& action) const override;
};

class MulliganActionPhase: public ActionPhase< MulliganActionPhase, actions::ActionID::MULLIGAN > {
  public:
   using base = ActionPhase< MulliganActionPhase, actions::ActionID::MULLIGAN >;
   using base::base;

   constexpr static actions::Phase phase_label = actions::Phase::MULLIGAN;
   bool is_valid(GameState& state, const actions::Action& action) const override;
   std::vector< actions::Action > valid_actions(const GameState& action) const override;
};

class ActionSystem: public ILogicSystem {
  public:
   ActionSystem(
      entt::registry& registry,
      uptr< ActionPhaseBase >&& phase = std::make_unique< DefaultActionPhase >())
       : ILogicSystem(registry), m_action_phase(std::move(phase))
   {
   }
   ~ActionSystem() = default;
   ActionSystem(const ActionSystem& other);
   ActionSystem(ActionSystem&& other) = default;
   ActionSystem& operator=(ActionSystem&& other) = delete;
   ActionSystem& operator=(const ActionSystem& other) = delete;

   [[nodiscard]] inline actions::Action request_action(const GameState& state) const
   {
      return m_action_phase->request_action(state);
   }
   inline bool invoke(actions::Action& action) { return m_action_phase->invoke(action); }
   [[nodiscard]] inline bool is_valid(GameState& state, const actions::Action& action) const
   {
      return m_action_phase->is_valid(state, action);
   }
   [[nodiscard]] inline std::vector< actions::Action > valid_actions(const GameState& state) const;

   template < actions::Phase phase, typename... Args >
   inline void transition(Args&&... args);
   void restore_previous_phase();

   auto& accepted_actions() const { return m_action_phase->accepted_actions(); }
   [[nodiscard]] auto phase() const { return m_action_phase->phase(); }

  private:
   uptr< ActionPhaseBase > m_action_phase;
   uptr< ActionPhaseBase > m_prev_phase = nullptr;
   std::vector< actions::Action > m_action_stack;
};

namespace helpers {
template < actions::Phase p >
struct phase_to_type;

template <>
struct phase_to_type< actions::Phase::DEFAULT > {
   constexpr static auto phase = actions::Phase::DEFAULT;
   using type = DefaultActionPhase;
};
template <>
struct phase_to_type< actions::Phase::COMBAT > {
   constexpr static auto phase = actions::Phase::COMBAT;
   using type = CombatActionPhase;
};
template <>
struct phase_to_type< actions::Phase::REPLACING > {
   constexpr static auto phase = actions::Phase::REPLACING;
   using type = ReplacingActionPhase;
};
template <>
struct phase_to_type< actions::Phase::TARGET > {
   constexpr static auto phase = actions::Phase::TARGET;
   using type = TargetActionPhase;
};

template < actions::Phase p >
using phase_to_type_t = typename phase_to_type< p >::type;

}  // namespace helpers

template < actions::Phase phase, typename... Args >
void ActionSystem::transition(Args&&... args)
{
   using PhaseType = helpers::phase_to_type_t< phase >;
   // move current invoker into previous
   m_prev_phase = std::move(m_action_phase);
   m_action_phase = std::make_unique< PhaseType >(this, std::forward< Args >(args)...);
}


#endif  // LORAINE_ACTION_SYSTEM_H
