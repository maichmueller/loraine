
#ifndef LORAINE_INPUT_SYSTEM_HPP
#define LORAINE_INPUT_SYSTEM_HPP

#include <set>
#include <utility>

#include "loraine/core/action.hpp"
#include "loraine/core/components.hpp"
#include "loraine/utils/types.h"
#include "system.hpp"

class GameState;
class InputHandlerBase;

class InputSystem: public ILogicSystem {
  public:
   enum class State { INITIATIVE = 0, CHOICE, ATTACK, BLOCK, COMBAT, MULLIGAN, REPLACE, TARGET };

   InputSystem(uptr< InputHandlerBase >&& handler) : m_handler(std::move(handler)) {}
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
   InputSystem* restore_handler();

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
       : m_input_system(act_system),
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
   virtual void handle(input::Action& action, GameState& state)
   {
      throw std::logic_error("Input handlers passed the action up to the base");
   }
   [[nodiscard]] virtual bool is_valid(const input::Action& action, const GameState& state)
      const = 0;
   [[nodiscard]] virtual std::vector< input::Action > valid_actions(
      const GameState& state) const = 0;

   auto& accepted_actions() { return m_accepted_actions; }
   [[nodiscard]] auto& accepted_actions() const { return m_accepted_actions; }
   virtual InputHandlerBase* clone() = 0;
   [[nodiscard]] auto state_id() const { return m_state_id; }

  protected:
   InputSystem* m_input_system;

   void change_turn(GameState& state);

  private:
   InputSystem::State m_state_id;
   std::set< input::ID > m_accepted_actions;
};

template < typename Derived, input::ID... AcceptedActions >
class InputHandler: public InputHandlerBase {
  public:
   using base = InputHandlerBase;
   using base::base;

   InputHandler(InputSystem* system)
       : base(system, Derived::state_id, std::set< input::ID >{AcceptedActions...})
   {
   }

   [[nodiscard]] virtual bool is_valid(const input::Action& action, const GameState& state)
      const = 0;
   [[nodiscard]] virtual std::vector< input::Action > valid_actions(
      const GameState& state) const = 0;

   inline InputHandlerBase* clone() override { return new Derived(static_cast< Derived& >(*this)); }
};

template < typename Derived, input::ID... AcceptedActions >
class SpellInputHandler: public InputHandler< Derived, AcceptedActions... > {
  public:
   using base = InputHandler< Derived, AcceptedActions... >;
   using base::base;

   SpellInputHandler(InputSystem* system)
       : base(
          system,
          Derived::state_id,
          std::set< input::ID >{
             input::ID::PLACE_SPELL,
             input::ID::RETRIEVE_SPELL,
             input::ID::BUTTONPRESS,
             AcceptedActions...})
   {
   }

  protected:
   /// handlers for specific actions. Does not transition the state, so any sub class using these
   /// handlers will need to transition themselves. These do not need to be used, thus are optional!
   void handle(input::PlaceSpellAction& action, GameState& state);
   void handle(input::RetrieveSpellAction& action, GameState& state);
   void handle(input::ButtonPressAction& action, GameState& state);
};

class InitiativeInputHandler:
    public SpellInputHandler<
       InitiativeInputHandler,
       input::ID::PLAY_FIELDCARD,
       input::ID::ADVANCE_UNIT > {
  public:
   using base = SpellInputHandler<
      InitiativeInputHandler,
      input::ID::PLAY_FIELDCARD,
      input::ID::ADVANCE_UNIT >;
   using base::base;

   constexpr static InputSystem::State state_id = InputSystem::State::INITIATIVE;

   void handle(input::Action& action, GameState& state) override;
   bool is_valid(const input::Action& action, const GameState& state) const override;

   std::vector< input::Action > valid_actions(const GameState& action) const override;
};

class CombatInputHandler: public SpellInputHandler< CombatInputHandler > {
  public:
   using base = SpellInputHandler< CombatInputHandler >;
   using base::base;

   constexpr static InputSystem::State state_id = InputSystem::State::COMBAT;

   void handle(input::Action& action, GameState& state) override;
   bool is_valid(const input::Action& action, const GameState& state) const override;

   std::vector< input::Action > valid_actions(const GameState& action) const override;
};

class AttackInputHandler:
    public SpellInputHandler<
       AttackInputHandler,
       input::ID::DRAG_ENEMY,
       input::ID::ADVANCE_UNIT,
       input::ID::RETREAT_UNIT > {
  public:
   using base = SpellInputHandler<
      AttackInputHandler,
      input::ID::DRAG_ENEMY,
      input::ID::ADVANCE_UNIT,
      input::ID::RETREAT_UNIT >;
   using base::base;

   constexpr static InputSystem::State state_id = InputSystem::State::ATTACK;

   void handle(input::Action& action, GameState& state) override;
   bool is_valid(const input::Action& action, const GameState& state) const override;

   std::vector< input::Action > valid_actions(const GameState& action) const override;
};

class BlockInputHandler: public SpellInputHandler< BlockInputHandler, input::ID::ADVANCE_UNIT > {
  public:
   using base = SpellInputHandler< BlockInputHandler, input::ID::ADVANCE_UNIT >;
   using base::base;

   constexpr static InputSystem::State state_id = InputSystem::State::BLOCK;

   void handle(input::Action& action, GameState& state) override;
   bool is_valid(const input::Action& action, const GameState& state) const override;

   std::vector< input::Action > valid_actions(const GameState& action) const override;
};

class TargetInputHandler:
    public InputHandler< TargetInputHandler, input::ID::CANCEL, input::ID::TARGETING > {
  public:
   using base = InputHandler< TargetInputHandler, input::ID::CANCEL, input::ID::TARGETING >;
   using base::base;

   TargetInputHandler(InputSystem* system, const std::vector< Targeter >& targeter)
       : base(system), m_targeter(targeter)
   {
   }

   constexpr static InputSystem::State state_id = InputSystem::State::TARGET;

   input::Action request_action(const GameState& state) const override;
   void handle(input::Action& action, GameState& state) override;
   bool is_valid(const input::Action& action, const GameState& state) const override;

   std::vector< input::Action > valid_actions(const GameState& action) const override;

  private:
   std::vector< Targeter > m_targeter;
};

class ChoiceInputHandler: public InputHandler< ChoiceInputHandler, input::ID::CHOICE > {
  public:
   using base = InputHandler< ChoiceInputHandler, input::ID::CHOICE >;
   using base::base;

   constexpr static InputSystem::State state_id = InputSystem::State::CHOICE;

   void handle(input::Action& action, GameState& state) override;
   bool is_valid(const input::Action& action, const GameState& state) const override;

   std::vector< input::Action > valid_actions(const GameState& action) const override;
};

class SkippableChoiceInputHandler:
    public InputHandler< SkippableChoiceInputHandler, input::ID::BUTTONPRESS, input::ID::CHOICE > {
  public:
   using base = InputHandler<
      SkippableChoiceInputHandler,
      input::ID::BUTTONPRESS,
      input::ID::CHOICE >;
   using base::base;

   constexpr static InputSystem::State state_id = InputSystem::State::CHOICE;

   void handle(input::Action& action, GameState& state) override;
   bool is_valid(const input::Action& action, const GameState& state) const override;

   std::vector< input::Action > valid_actions(const GameState& action) const override;
};

class MulliganInputHandler: public InputHandler< MulliganInputHandler, input::ID::MULLIGAN > {
  public:
   using base = InputHandler< MulliganInputHandler, input::ID::MULLIGAN >;
   using base::base;

   constexpr static InputSystem::State state_id = InputSystem::State::MULLIGAN;

   void handle(input::Action& action, GameState& state) override;
   bool is_valid(const input::Action& action, const GameState& state) const override;
   std::vector< input::Action > valid_actions(const GameState& action) const override;
};

template < class NewStateType, typename... Args >
InputSystem* InputSystem::transition(Args&&... args)
{
   static_assert(
      utils::is_any_v<
         NewStateType,
         InitiativeInputHandler,
         AttackInputHandler,
         BlockInputHandler,
         ChoiceInputHandler,
         SkippableChoiceInputHandler,
         CombatInputHandler,
         TargetInputHandler,
         MulliganInputHandler >,
      "New Input-System type unknown.");
   // move current invoker into previous
   m_prev_handler = std::move(m_handler);
   m_handler = std::make_unique< NewStateType >(this, std::forward< Args >(args)...);
   return this;
}

#include "loraine/core/gamestate.h"
#include "loraine/core/systems/board_system.hpp"
#include "loraine/core/systems/play_system.hpp"
#include "loraine/core/systems/target_system.hpp"

template < typename Derived, input::ID... AcceptedActions >
void SpellInputHandler< Derived, AcceptedActions... >::handle(
   input::PlaceSpellAction& action,
   GameState& state)
{
   auto& board_system = state.get< BoardSystem >();
   auto& play_system = state.get< PlaySystem >();
   auto& kw_system = state.get< KeywordSystem >();
   auto& target_system = state.get< TargetSystem >();
   auto& registry = state.registry();
   auto spell = board_system.at< Zone::HAND >(action.hand_index);

   // TODO: The actual game does not fully move the card, until the targeting is done. If during the
   //  targeting, the player cancels placing the spell, then the spell is moved back into the hand
   //  AT THE POSITION IT HELD BEFORE! Only once the spell has been fully placed, i.e. including
   //  any targeting duties, is the spell moved completely from hand onto the stack and returns to
   //  the end of the hand, if returned from the spell stack.

   board_system.move_to< Zone::SPELLSTACK >(spell, board_system.size< Zone::SPELLSTACK >());
   play_system.to_play_queue(spell);
   if(auto targets = target_system.manual_targeters< events::EventID::CAST >(state, spell);
      not targets.empty()) {
      base::m_input_system->template transition< TargetInputHandler >(
         state.registry().get< Targeter >(spell), targets);
   } else if(registry.get< KeywordMap >(spell).has_any_of(
                std::array{Keyword::BURST, Keyword::FOCUS})) {
      play_system.play(true);
   }
}

template < typename Derived, input::ID... AcceptedActions >
void SpellInputHandler< Derived, AcceptedActions... >::handle(
   input::RetrieveSpellAction& action,
   GameState& state)
{
   auto& board_system = state.get< BoardSystem >();
   auto& play_system = state.get< PlaySystem >();
   auto& kw_system = state.get< KeywordSystem >();
   auto& target_system = state.get< TargetSystem >();
   auto& registry = state.registry();

   auto card = board_system.at< Zone::SPELLSTACK >(action.stack_index);
   board_system.move_to< Zone::HAND >(card);
   if(registry.all_of< EffectVector< events::EventID::CAST > >(card)) {
      algo::apply(
         [&](auto& effect) {
            target_system.reset_targets(effect);
            effect.condition(state);
         },
         registry.get< EffectVector< events::EventID::CAST > >(card));
   }
}

template < typename Derived, input::ID... AcceptedActions >
void SpellInputHandler< Derived, AcceptedActions... >::handle(
   input::ButtonPressAction& action,
   GameState& state)
{
   auto& play_system = state.get< PlaySystem >();

   // play all spells on the stack that have been placed
   play_system.play(false);
}

#endif  // LORAINE_INPUT_SYSTEM_HPP
