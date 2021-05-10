

#ifndef LORAINE_ACTION_HPP
#define LORAINE_ACTION_HPP

#include <entt/entt.hpp>
#include <map>
#include <utility>
#include <variant>

#include "gamedefs.h"
#include "loraine/utils/utils.h"

class GameState;

namespace input {

class Action;

enum class ID {
   BUTTONPRESS,
   ADVANCE_UNIT,
   CANCEL,
   CHOICE,
   DRAG_ENEMY,
   PLACE_SPELL,
   MULLIGAN,
   PLAY_FIELDCARD,
   RETREAT_UNIT,
   TARGETING
};

template < ID action_id >
struct ActionIDType {
   constexpr static ID id = action_id;
};

template < ID id_ >
struct ActionBase {
   constexpr static ID id = id_;

   ActionBase(Team team) noexcept : team(team) {}

   Team team;
   bool completed = false;
};

/**
 * This is the action for whatever happens when the is pressed in the game.
 * What this action does precisely depends on the specific circumstances.
 */
struct ButtonPressAction: public ActionBase< ID::BUTTONPRESS > {
   using base = ActionBase< ID::BUTTONPRESS >;
   using base::base;
};

/**
 * Action for playing a fieldcard
 */
struct PlayFieldcardAction: public ActionBase< ID::PLAY_FIELDCARD > {
   using base = ActionBase< ID::PLAY_FIELDCARD >;

   PlayFieldcardAction(Team team, entt::entity card) noexcept
       : ActionBase(team), card(card), target_index(std::nullopt)
   {
   }
   PlayFieldcardAction(Team team, entt::entity card, size_t target_index) noexcept
       : ActionBase(team), card(card), target_index(target_index)
   {
   }

   // the index of the fieldcard in hand to play
   entt::entity card;
   // the index in the camp to place the fieldcard (should be set automatically, unless full)
   std::optional< size_t > target_index;
};


/**
 * Action for playing a fieldcard
 */
struct ChoiceAction: public ActionBase< ID::CHOICE > {
   using base = ActionBase< ID::CHOICE >;

   ChoiceAction(Team team, size_t n_choices, size_t choice) noexcept
       : ActionBase(team), n_choices(n_choices), choice(choice)
   {
   }

   size_t n_choices;
   size_t choice;
};

struct CancelAction: public ActionBase< ID::CANCEL > {
   using base = ActionBase< ID::CANCEL >;
   using base::base;
};

struct PlaceSpellAction: public ActionBase< ID::PLACE_SPELL > {
  public:
   using base = ActionBase< ID::PLACE_SPELL >;

   PlaceSpellAction(Team team, size_t hand_index, bool to_stack) noexcept
       : ActionBase(team), hand_index(hand_index), to_stack(to_stack)
   {
   }

   // the index of the spell in hand to place
   size_t hand_index;
   bool to_stack;
};

struct AdvanceUnitAction: public ActionBase< ID::ADVANCE_UNIT > {
   // Action for moving units from the camp (index-based) onto the battlefield
   using base = ActionBase< ID::ADVANCE_UNIT >;

   AdvanceUnitAction(Team team, std::vector< size_t > indices_vec)
       : ActionBase(team), indices(std::move(indices_vec))
   {
   }

   std::vector< size_t > indices;
};

struct DragEnemyAction: public ActionBase< ID::DRAG_ENEMY > {
   // Drags an opponent unit either from the camp onto the battlefield (e.g. via challenger or
   // vulnerable keyword) or vice versa.
   using base = ActionBase< ID::DRAG_ENEMY >;

   DragEnemyAction(Team team, bool to_bf, size_t from, size_t to) noexcept
       : ActionBase(team), to_bf(to_bf), from(from), to(to)
   {
   }

   bool to_bf;
   size_t from;
   size_t to;
};

/**
 * Action for deciding which cards to replace in the initial draw
 */
struct MulliganAction: public ActionBase< ID::MULLIGAN > {
  public:
   using base = ActionBase< ID::MULLIGAN >;

   explicit MulliganAction(Team team, std::vector< bool > replace)
       : ActionBase(team), replace(std::move(replace))
   {
   }

   // the positions on the battlefield the units take
   std::vector< bool > replace;
};

struct TargetingAction: public ActionBase< ID::TARGETING > {
   using base = ActionBase< ID::TARGETING >;

   TargetingAction(Team team, std::vector< entt::entity > targets)
       : ActionBase(team), targets(std::move(targets))
   {
   }
   // the selected targets
   std::vector< entt::entity > targets;
};


struct RetreatUnitAction: public ActionBase< ID::RETREAT_UNIT > {
   // Action for moving units from the battlefield (index-based) back to the camp
   using base = ActionBase< ID::RETREAT_UNIT >;

   RetreatUnitAction(Team team, std::vector< size_t > indices_vec)
       : ActionBase(team), indices(std::move(indices_vec))
   {
   }
   std::vector< size_t > indices;
};


class Action {
  private:
   using ActionVariant = std::variant<
      ButtonPressAction,
      CancelAction,
      ChoiceAction,
      DragEnemyAction,
      MulliganAction,
      PlaceSpellAction,
      AdvanceUnitAction,
      PlayFieldcardAction,
      RetreatUnitAction,
      TargetingAction>;

  public:
   Action(ActionVariant&& action) noexcept : m_action_detail(std::move(action)) {}

   template < typename DetailType >
   inline auto& detail()
   {
      return std::get< DetailType >(m_action_detail);
   }
   // default for not providing any template type and therefore simply getting the entire variant
   inline auto& detail() { return m_action_detail; }

   template < typename DetailType >
   [[nodiscard]] inline auto& detail() const
   {
      return std::get< DetailType >(m_action_detail);
   }
   // default for not providing any template type and therefore simply getting the entire variant
   [[nodiscard]] inline auto& detail() const { return m_action_detail; }

   [[nodiscard]] inline auto team() const
   {
      return std::visit([](const auto& action) { return action.team; }, m_action_detail);
   }
   [[nodiscard]] constexpr inline auto id() const
   {
      return std::visit([](const auto& action) { return action.id; }, m_action_detail);
   }
   constexpr inline void mark_complete()
   {
      std::visit([](auto& action) { action.completed = true; }, m_action_detail);
   }

   template < ID id_ >
   [[nodiscard]] constexpr inline bool is() const
   {
      return id() == id_;
   }

  private:
   // the type of action performed
   ActionVariant m_action_detail;
};

}  // namespace input

#endif  // LORAINE_ACTION_HPP
