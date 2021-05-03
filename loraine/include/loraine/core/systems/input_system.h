
#ifndef LORAINE_INPUT_SYSTEM_H
#define LORAINE_INPUT_SYSTEM_H

#include <set>
#include <utility>

#include "loraine/core/action.h"
#include "loraine/utils/types.h"
#include "system.hpp"

class GameState;
class InputHandlerBase;

class InputSystem: public ILogicSystem {
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

   InputSystem(entt::registry& registry, uptr< InputHandlerBase >&& handler)
       : ILogicSystem(registry), m_handler(std::move(handler))
   {
   }
   ~InputSystem() = default;
   InputSystem(const InputSystem& other);
   InputSystem(InputSystem&& other) = default;
   InputSystem& operator=(InputSystem&& other) = delete;
   InputSystem& operator=(const InputSystem& other) = delete;

   [[nodiscard]] input::Action request_action(const GameState& state) const;
   bool invoke(input::Action& action);
   [[nodiscard]] bool is_valid(GameState& state, const input::Action& action) const;
   [[nodiscard]] inline std::vector< input::Action > valid_actions(const GameState& state) const;

   template < class NewStateType, typename... Args >
   InputSystem* transition(Args&&... args);
   InputSystem* restore_previous_handler();

   auto& accepted_actions() const;
   [[nodiscard]] auto handler() const;

  private:
   uptr< InputHandlerBase > m_handler;
   uptr< InputHandlerBase > m_prev_handler = nullptr;
   std::vector< input::Action > m_action_buffer;
};

class InputHandlerBase {
  public:
   InputHandlerBase(
      InputSystem* act_system,
      InputSystem::State state_id,
      std::set< input::ID > accepted_actions)
       : m_action_system(act_system),
         m_state_id(state_id),
         m_accepted_actions(std::move(accepted_actions))
   {
   }
   virtual ~InputHandlerBase() = default;
   InputHandlerBase(const InputHandlerBase& other) = default;
   InputHandlerBase(InputHandlerBase&& other) = default;
   InputHandlerBase& operator=(InputHandlerBase&& other) = delete;
   InputHandlerBase& operator=(const InputHandlerBase& other) = delete;

   [[nodiscard]] virtual input::Action request_action(const GameState& state) const;
   virtual bool handle(input::Action& action)
   {
      throw std::logic_error("Action handlers passed the action up to the base");
   }
   [[nodiscard]] virtual bool is_valid(GameState& state, const input::Action& action) const = 0;
   [[nodiscard]] virtual std::vector< input::Action > valid_actions(
      const GameState& state) const = 0;

   auto& accepted_actions() { return m_accepted_actions; }
   [[nodiscard]] auto& accepted_actions() const { return m_accepted_actions; }
   virtual InputHandlerBase* clone() = 0;
   [[nodiscard]] auto state_id() const { return m_state_id; }

  private:
   InputSystem* m_action_system;
   InputSystem::State m_state_id;
   std::set< input::ID > m_accepted_actions;
};

template < typename Derived, input::ID... AcceptedActions >
class ActionHandler: public InputHandlerBase {
  public:
   using base = InputHandlerBase;
   using base::base;

   ActionHandler(InputSystem* system)
       : base(system, Derived::state_id, std::set< input::ID >{AcceptedActions...})
   {
   }
   template < typename... Args >
   ActionHandler(InputSystem* system, Args... args)
       : base(
          system,
          Derived::state_id,
          std::forward< Args >(args)...,
          std::set< input::ID >{AcceptedActions...})
   {
   }

   [[nodiscard]] virtual bool is_valid(GameState& state, const input::Action& action) const = 0;
   [[nodiscard]] virtual std::vector< input::Action > valid_actions(
      const GameState& state) const = 0;

   inline InputHandlerBase* clone() override { return new Derived(static_cast< Derived& >(*this)); }
};

template < typename Derived, input::ID... AcceptedActions >
class SpellAllowingActionHandler: public ActionHandler< Derived, AcceptedActions... > {
  public:
   using base = ActionHandler< Derived, AcceptedActions... >;
   using base::base;

   SpellAllowingActionHandler(InputSystem* system)
       : base(
          system,
          Derived::state_id,
          std::set< input::ID >{
             input::ID::PLACE_SPELL,
             input::ID::PLAY_SPELL,
             input::ID::ACCEPT,
             AcceptedActions...})
   {
   }
   template < typename... Args >
   SpellAllowingActionHandler(InputSystem* system, Args... args)
       : base(
          system,
          Derived::state_id,
          std::forward< Args >(args)...,
          std::set< input::ID >{
             input::ID::PLACE_SPELL,
             input::ID::PLAY_SPELL,
             input::ID::ACCEPT,
             AcceptedActions...})
   {
   }

  protected:
   /// handlers for specific actions. Does not transition the state, so any sub class using these
   /// handlers will need to transition themselves
   void handle(const input::PlaceSpellAction& action);
   void handle(const input::PlayFieldcardAction& action);
   void handle(const input::AcceptAction& action);
};

class IdleActionHandler:
    public SpellAllowingActionHandler<
       IdleActionHandler,
       input::ID::PASS,
       input::ID::PLAY_FIELDCARD,
       input::ID::DRAG_ENEMY,
       input::ID::PLACE_UNIT > {
  public:
   using base = SpellAllowingActionHandler<
      IdleActionHandler,
      input::ID::PASS,
      input::ID::PLAY_FIELDCARD,
      input::ID::DRAG_ENEMY,
      input::ID::PLACE_UNIT >;
   using base::base;

   constexpr static InputSystem::State state_id = InputSystem::State::IDLE;

   bool is_valid(GameState& state, const input::Action& action) const override;

   std::vector< input::Action > valid_actions(const GameState& action) const override;
};

class TargetActionHandler:
    public ActionHandler<
       TargetActionHandler,
       input::ID::ACCEPT,
       input::ID::CANCEL,
       input::ID::TARGETING > {
  public:
   using base = ActionHandler<
      TargetActionHandler,
      input::ID::ACCEPT,
      input::ID::CANCEL,
      input::ID::TARGETING >;
   using base::base;

   constexpr static InputSystem::State state_id = InputSystem::State::TARGET;
   [[nodiscard]] input::Action request_action(const GameState& state) const override;
   bool is_valid(GameState& state, const input::Action& action) const override;

   std::vector< input::Action > valid_actions(const GameState& action) const override;
};

class ReplacingActionHandler:
    public ActionHandler< ReplacingActionHandler, input::ID::CANCEL, input::ID::REPLACE > {
  public:
   using base = ActionHandler< ReplacingActionHandler, input::ID::CANCEL, input::ID::REPLACE >;
   using base::base;

   constexpr static InputSystem::State state_id = InputSystem::State::REPLACING;
   [[nodiscard]] input::Action request_action(const GameState& state) const override;
   bool is_valid(GameState& state, const input::Action& action) const override;

   std::vector< input::Action > valid_actions(const GameState& action) const override;
};

class MulliganActionHandler: public ActionHandler< MulliganActionHandler, input::ID::MULLIGAN > {
  public:
   using base = ActionHandler< MulliganActionHandler, input::ID::MULLIGAN >;
   using base::base;

   constexpr static InputSystem::State state_id = InputSystem::State::MULLIGAN;
   bool is_valid(GameState& state, const input::Action& action) const override;
   std::vector< input::Action > valid_actions(const GameState& action) const override;
};

template < class NewStateType, typename... Args >
InputSystem* InputSystem::transition(Args&&... args)
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

#endif  // LORAINE_INPUT_SYSTEM_H
