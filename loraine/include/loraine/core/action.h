

#ifndef LORAINE_ACTION_H
#define LORAINE_ACTION_H

#include <entt/entt.hpp>
#include <map>
#include <utility>
#include <variant>

#include "gamedefs.h"
#include "loraine/utils/utils.h"

class GameState;

namespace actions {

class Action;

enum class ActionID {
   ACCEPT,
   CANCEL,
   CHOICE,
   DRAG_ENEMY,
   PLACE_UNIT,
   PLACE_SPELL,
   MULLIGAN,
   PASS,
   PLAY_FIELDCARD,
   PLAY_SPELL,
   REPLACE,
   TARGETING,
   SKIP
};

enum class ActionRank { PRIMITIVE = 0, MACRO };

template < ActionID action_id >
class ActionIDType {
   constexpr static ActionRank _determine_rank()
   {
      constexpr std::array macro_ids{ActionID::PLAY_FIELDCARD, ActionID::PLAY_SPELL};

      if constexpr(not algo::any_of(
                      macro_ids, [&](const ActionID& label) { return label == action_id; })) {
         return ActionRank::MACRO;
      }
      return ActionRank::PRIMITIVE;
   }

  public:
   constexpr static ActionID id = action_id;
   constexpr static ActionRank rank = _determine_rank();
};

template < ActionID id_ >
struct ActionBase {
   constexpr static ActionID id = id_;

   ActionBase(Team team) noexcept : team(team) {}

   Team team;
   bool completed = false;
};

/**
 * This is the action for accepting the outcome of whatever
 * the current state is. Counts as pass if nothing has been done
 */
struct AcceptAction: public ActionBase< ActionID::ACCEPT > {
   using base = ActionBase< ActionID::ACCEPT >;
   using base::base;
};

/**
 * Action for playing a fieldcard
 */
struct PlayFieldcardAction: public ActionBase< ActionID::PLAY_FIELDCARD > {
   using base = ActionBase< ActionID::PLAY_FIELDCARD >;

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
 * Action for playing spells
 */
struct PlaySpellAction: public ActionBase< ActionID::PLAY_SPELL > {
   using base = ActionBase< ActionID::PLAY_SPELL >;

   PlaySpellAction(Team team, const std::vector< entt::entity >& spells) noexcept
       : ActionBase(team), spells(spells)
   {
   }

   // the spells to play
   std::vector< entt::entity > spells;
};

/**
 * Action for playing a fieldcard
 */
struct ChoiceAction: public ActionBase< ActionID::CHOICE > {
   using base = ActionBase< ActionID::CHOICE >;

   ChoiceAction(Team team, size_t n_choices, size_t choice) noexcept
       : ActionBase(team), n_choices(n_choices), choice(choice)
   {
   }

   size_t n_choices;
   size_t choice;
};

struct CancelAction: public ActionBase< ActionID::CANCEL > {
   using base = ActionBase< ActionID::CANCEL >;
   using base::base;
};

struct PlaceSpellAction: public ActionBase< ActionID::PLACE_SPELL > {
  public:
   using base = ActionBase< ActionID::PLACE_SPELL >;

   PlaceSpellAction(Team team, size_t hand_index, bool to_stack) noexcept
       : ActionBase(team), hand_index(hand_index), to_stack(to_stack)
   {
   }

   // the index of the spell in hand to place
   size_t hand_index;
   bool to_stack;
};

struct PlaceUnitAction: public ActionBase< ActionID::PLACE_UNIT > {
   // Action for moving units either from the camp (index-based) onto the battlefield or from the
   // battlefield onto the camp
   using base = ActionBase< ActionID::PLACE_UNIT >;

   PlaceUnitAction(Team team, bool to_bf, std::vector< size_t > indices_vec)
       : ActionBase(team), to_bf(to_bf), indices_vec(std::move(indices_vec))
   {
   }

   bool to_bf;
   std::vector< size_t > indices_vec;
};

struct DragEnemyAction: public ActionBase< ActionID::DRAG_ENEMY > {
   // Drags an opponent unit either from the camp onto the battlefield (e.g. via challenger or
   // vulnerable keyword) or vice versa.
   using base = ActionBase< ActionID::DRAG_ENEMY >;

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
struct MulliganAction: public ActionBase< ActionID::MULLIGAN > {
  public:
   using base = ActionBase< ActionID::MULLIGAN >;

   explicit MulliganAction(Team team, std::vector< bool > replace)
       : ActionBase(team), replace(std::move(replace))
   {
   }

   // the positions on the battlefield the units take
   std::vector< bool > replace;
};

struct TargetingAction: public ActionBase< ActionID::TARGETING > {
   using base = ActionBase< ActionID::TARGETING >;

   TargetingAction(Team team, std::vector< entt::entity > targets)
       : ActionBase(team), targets(std::move(targets))
   {
   }
   // the selected targets
   std::vector< entt::entity > targets;
};

struct ReplaceAction: public ActionBase< ActionID::REPLACE > {
   using base = ActionBase< ActionID::REPLACE >;

   ReplaceAction(Team team, size_t replace_index) : ActionBase(team), replace_index(replace_index)
   {
   }

   // the selected target
   size_t replace_index;
};

struct SkipAction: public ActionBase< ActionID::SKIP > {
   using base = ActionBase< ActionID::SKIP >;
};

class Action {
  private:
   using ActionVariant = std::variant<
      AcceptAction,
      CancelAction,
      ChoiceAction,
      DragEnemyAction,
      MulliganAction,
      PlaceSpellAction,
      PlaceUnitAction,
      PlayFieldcardAction,
      PlaySpellAction,
      TargetingAction,
      SkipAction >;

  public:
   Action(ActionVariant&& action) noexcept : m_action_detail(std::move(action)) {}

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

   template < ActionID id_ >
   [[nodiscard]] constexpr inline bool is() const
   {
      return id() == id_;
   }

  private:
   // the type of action performed
   ActionVariant m_action_detail;
};

}  // namespace actions

#endif  // LORAINE_ACTION_H
