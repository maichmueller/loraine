
#ifndef LORAINE_ACTION_SYSTEM_H
#define LORAINE_ACTION_SYSTEM_H

#include <set>
#include <utility>

#include "loraine/core/action.h"
#include "loraine/utils/types.h"
#include "system.hpp"

class GameState;
class ActionHandlerBase;

class ActionSystem: public ILogicSystem {
  public:
   enum class State {
      IDLE = 0,
      CHOICE,
      ATTACK,
      BLOCK,
      COMBAT,
      MULLIGAN,
      REPLACING,
      SPELLFIGHT,
      TARGET
   };

   ActionSystem(entt::registry& registry, uptr< ActionHandlerBase >&& handler)
       : ILogicSystem(registry), m_handler(std::move(handler))
   {
   }
   ~ActionSystem() = default;
   ActionSystem(const ActionSystem& other);
   ActionSystem(ActionSystem&& other) = default;
   ActionSystem& operator=(ActionSystem&& other) = delete;
   ActionSystem& operator=(const ActionSystem& other) = delete;

   [[nodiscard]] actions::Action request_action(const GameState& state) const;
   bool invoke(actions::Action& action);
   [[nodiscard]] bool is_valid(GameState& state, const actions::Action& action) const;
   [[nodiscard]] inline std::vector< actions::Action > valid_actions(const GameState& state) const;

   template < class NewStateType, typename... Args >
   ActionSystem* transition(Args&&... args);
   ActionSystem* restore_previous_handler();

   auto& accepted_actions() const;
   [[nodiscard]] auto handler() const;

  private:
   uptr< ActionHandlerBase > m_handler;
   uptr< ActionHandlerBase > m_prev_handler = nullptr;
   std::vector< actions::Action> m_action_buffer;
};


class ActionHandlerBase {
  public:
   ActionHandlerBase(
      ActionSystem* act_system,
      ActionSystem::State state_id,
      std::set< actions::ActionID > accepted_actions)
       : m_action_system(act_system),
         m_state_id(state_id),
         m_accepted_actions(std::move(accepted_actions))
   {
   }
   virtual ~ActionHandlerBase() = default;
   ActionHandlerBase(const ActionHandlerBase& other) = default;
   ActionHandlerBase(ActionHandlerBase&& other) = default;
   ActionHandlerBase& operator=(ActionHandlerBase&& other) = delete;
   ActionHandlerBase& operator=(const ActionHandlerBase& other) = delete;

   [[nodiscard]] virtual actions::Action request_action(const GameState& state) const;
   virtual bool handle(actions::Action& action)
   {
      throw std::logic_error("Action handlers passed the action up to the base");
   }
   [[nodiscard]] virtual bool is_valid(GameState& state, const actions::Action& action) const = 0;
   [[nodiscard]] virtual std::vector< actions::Action > valid_actions(
      const GameState& state) const = 0;

   auto& accepted_actions() { return m_accepted_actions; }
   [[nodiscard]] auto& accepted_actions() const { return m_accepted_actions; }
   virtual ActionHandlerBase* clone() = 0;
   [[nodiscard]] auto state_id() const { return m_state_id; }

  private:
   ActionSystem* m_action_system;
   ActionSystem::State m_state_id;
   std::set< actions::ActionID > m_accepted_actions;
};

template < typename Derived, actions::ActionID... AcceptedActions >
class ActionHandler: public ActionHandlerBase {
  public:
   using base = ActionHandlerBase;
   using base::base;

   ActionHandler(ActionSystem* system)
       : base(system, Derived::state_id, std::set< actions::ActionID >{AcceptedActions...})
   {
   }
   template < typename... Args >
   ActionHandler(ActionSystem* system, Args... args)
       : base(
          system,
          Derived::state_id,
          std::forward< Args >(args)...,
          std::set< actions::ActionID >{AcceptedActions...})
   {
   }

   [[nodiscard]] virtual bool is_valid(GameState& state, const actions::Action& action) const = 0;
   [[nodiscard]] virtual std::vector< actions::Action > valid_actions(
      const GameState& state) const = 0;

   inline ActionHandlerBase* clone() override
   {
      return new Derived(static_cast< Derived& >(*this));
   }
};

template < typename Derived, actions::ActionID... AcceptedActions >
class SpellAllowingActionHandler: public ActionHandler< Derived, AcceptedActions... > {
  public:
   using base = ActionHandler< Derived, AcceptedActions... >;
   using base::base;

   SpellAllowingActionHandler(ActionSystem* system)
       : base(
          system,
          Derived::state_id,
          std::set< actions::ActionID >{
             actions::ActionID::PLACE_SPELL,
             actions::ActionID::PLAY_SPELL,
             actions::ActionID::ACCEPT,
             AcceptedActions...})
   {
   }
   template < typename... Args >
   SpellAllowingActionHandler(ActionSystem* system, Args... args)
       : base(
          system,
          Derived::state_id,
          std::forward< Args >(args)...,
          std::set< actions::ActionID >{
             actions::ActionID::PLACE_SPELL,
             actions::ActionID::PLAY_SPELL,
             actions::ActionID::ACCEPT,
             AcceptedActions...})
   {
   }

  protected:
   /// handlers for specific actions. Does not transition the state, so any sub class using these
   /// handlers will need to transition themselves
   void handle(const actions::PlaceSpellAction& action);
   void handle(const actions::Pla& action);
   void handle(const actions::AcceptAction& action);
};

class IdleActionHandler:
    public SpellAllowingActionHandler<
       IdleActionHandler,
       actions::ActionID::PASS,
       actions::ActionID::PLAY_FIELDCARD,
       actions::ActionID::DRAG_ENEMY,
       actions::ActionID::PLACE_UNIT > {
  public:
   using base = SpellAllowingActionHandler<
      IdleActionHandler,
      actions::ActionID::PASS,
      actions::ActionID::PLAY_FIELDCARD,
      actions::ActionID::DRAG_ENEMY,
      actions::ActionID::PLACE_UNIT>;
   using base::base;

   constexpr static ActionSystem::State state_id = ActionSystem::State::IDLE;

   bool is_valid(GameState& state, const actions::Action& action) const override;

   std::vector< actions::Action > valid_actions(const GameState& action) const override;
};

class TargetActionHandler:
    public ActionHandler<
       TargetActionHandler,
       actions::ActionID::ACCEPT,
       actions::ActionID::CANCEL,
       actions::ActionID::TARGETING > {
  public:
   using base = ActionHandler<
      TargetActionHandler,
      actions::ActionID::ACCEPT,
      actions::ActionID::CANCEL,
      actions::ActionID::TARGETING >;
   using base::base;

   constexpr static ActionSystem::State state_id = ActionSystem::State::TARGET;
   [[nodiscard]] actions::Action request_action(const GameState& state) const override;
   bool is_valid(GameState& state, const actions::Action& action) const override;

   std::vector< actions::Action > valid_actions(const GameState& action) const override;
};

class ReplacingActionHandler:
    public ActionHandler<
       ReplacingActionHandler,
       actions::ActionID::CANCEL,
       actions::ActionID::REPLACE > {
  public:
   using base = ActionHandler<
      ReplacingActionHandler,
      actions::ActionID::CANCEL,
      actions::ActionID::REPLACE >;
   using base::base;

   constexpr static ActionSystem::State state_id = ActionSystem::State::REPLACING;
   [[nodiscard]] actions::Action request_action(const GameState& state) const override;
   bool is_valid(GameState& state, const actions::Action& action) const override;

   std::vector< actions::Action > valid_actions(const GameState& action) const override;
};

class MulliganActionHandler:
    public ActionHandler< MulliganActionHandler, actions::ActionID::MULLIGAN > {
  public:
   using base = ActionHandler< MulliganActionHandler, actions::ActionID::MULLIGAN >;
   using base::base;

   constexpr static ActionSystem::State state_id = ActionSystem::State::MULLIGAN;
   bool is_valid(GameState& state, const actions::Action& action) const override;
   std::vector< actions::Action > valid_actions(const GameState& action) const override;
};


template < class NewStateType, typename... Args >
ActionSystem* ActionSystem::transition(Args&&... args)
{
   //   using PhaseType = helpers::phase_to_type_t< phase >;
   static_assert(
      utils::is_any_v< NewStateType, All Types here >,
      "New Action System State type not supported.");
   // move current invoker into previous
   m_prev_handler = std::move(m_handler);
   m_handler = std::make_unique< NewStateType >(this, std::forward< Args >(args)...);
   return this;
}

#endif  // LORAINE_ACTION_SYSTEM_H
