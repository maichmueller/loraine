

#ifndef LORAINE_ACTION_H
#define LORAINE_ACTION_H

#include <map>
#include <utility>
#include <variant>

#include "utils/utils.h"
#include "core/gamedefs.h"
#include "core/targetable.h"
#include "utils/types.h"

class Card;
class FieldCard;
class Spell;
class State;

namespace actions {
enum class ActionLabel {
   ACCEPT,
   CANCEL,
   CHOICE,
   DRAG_ENEMY,
   PLACE_UNIT,
   PLACE_SPELL,
   MULLIGAN,
   PASS,
   PLAY,
   TARGETING
};

template < ActionLabel action_label >
struct ActionType {
   constexpr static ActionLabel type = action_label;
};

template < typename Derived, typename ActionT >
class ActionBase: public utils::CRTP< ActionBase, Derived, ActionT > {
  public:
   constexpr static ActionLabel label = ActionT::type;

   explicit ActionBase(Team team) noexcept : m_team(team) {}

   [[nodiscard]] auto team() const { return m_team; }

   bool execute(State& state) { return this->derived()->execute_impl(state); }

  private:
   Team m_team;
};

/**
 * Action for passing
 */
class PassAction: public ActionBase< PassAction, actions::ActionType< ActionLabel::PASS > > {
  public:
   using base = ActionBase< PassAction, actions::ActionType< ActionLabel::PASS > >;
   using base::base;

   bool execute_impl(State& state);
};
/**
 * This is the action for accepting the outcome of whatever
 * the current state is.
 */
class AcceptAction: public ActionBase< AcceptAction, actions::ActionType< ActionLabel::ACCEPT > > {
  public:
   using base = ActionBase< AcceptAction, actions::ActionType< ActionLabel::ACCEPT > >;
   using base::base;

   bool execute_impl(State& state);

  private:
   void _resolve_battle();
   void _resolve_spell_stack(State& state, bool burst);
};
/**
 * Action for playing a fieldcard
 */
class PlayAction: public ActionBase< PlayAction, actions::ActionType< ActionLabel::PLAY > > {
  public:
   using base = ActionBase< PlayAction, actions::ActionType< ActionLabel::PLAY > >;
   using base::base;

   explicit PlayAction(Team team, sptr< FieldCard > card_played) noexcept
       : ActionBase(team), m_card_played(std::move(card_played))
   {
   }
   [[nodiscard]] inline auto card_played() const { return m_card_played; }

   bool execute_impl(State& state);

  private:
   // the actual card that was played
   sptr< FieldCard > m_card_played;
};

/**
 * Action for playing a fieldcard
 */
class ChoiceAction: public ActionBase< ChoiceAction, actions::ActionType< ActionLabel::CHOICE > > {
  public:
   using base = ActionBase< ChoiceAction, actions::ActionType< ActionLabel::CHOICE > >;
   using base::base;

   explicit ChoiceAction(Team team, size_t n_choices, size_t choice) noexcept
       : ActionBase(team), m_nr_choices(n_choices), m_choice(choice)
   {
   }
   [[nodiscard]] inline auto n_choices() const { return m_nr_choices; }
   [[nodiscard]] inline auto choice() const { return m_choice; }

   bool execute_impl(State& state);

  private:
   size_t m_nr_choices;
   size_t m_choice;
};

class CancelAction: public ActionBase< CancelAction, actions::ActionType< ActionLabel::CANCEL > > {
  public:
   using base = ActionBase< CancelAction, actions::ActionType< ActionLabel::CANCEL > >;
   using base::base;

   bool execute_impl(State& state);
};

class PlaceSpellAction:
    public ActionBase< PlaceSpellAction, actions::ActionType< ActionLabel::PLACE_SPELL > > {
  public:
   using base = ActionBase< PlaceSpellAction, actions::ActionType< ActionLabel::PLACE_SPELL > >;
   using base::base;

   PlaceSpellAction(Team team, sptr< Spell > spell, bool to_stack) noexcept
       : ActionBase(team), m_spell(std::move(spell)), m_to_stack(to_stack)
   {
   }
   [[nodiscard]] inline auto spell() const { return m_spell; }
   [[nodiscard]] inline auto to_stack() const { return m_to_stack; }

   bool execute_impl(State& state);

  private:
   sptr< Spell > m_spell;
   bool m_to_stack;
};


class PlaceUnitAction:
    public ActionBase< PlaceUnitAction, actions::ActionType< ActionLabel::PLACE_UNIT > > {
   // Action for moving units either from the camp (index-based) onto the battlefield or from the
   // battlefield onto the camp
  public:
   using base = ActionBase< PlaceUnitAction, actions::ActionType< ActionLabel::PLACE_UNIT > >;
   using base::base;

   PlaceUnitAction(Team team, bool to_bf, std::vector< size_t > indices_vec)
       : ActionBase(team), m_to_bf(to_bf), m_indices_vec(std::move(indices_vec))
   {
   }
   [[nodiscard]] inline auto to_bf() const { return m_to_bf; }
   [[nodiscard]] inline auto indices_vec() const { return m_indices_vec; }

   bool execute_impl(State& state);

  private:
   bool m_to_bf;
   std::vector< size_t > m_indices_vec;
};

class DragEnemyAction:
    public ActionBase< DragEnemyAction, actions::ActionType< ActionLabel::DRAG_ENEMY > > {
   // Drags an opponent unit either from the camp onto the battlefield (e.g. via challenger or
   // vulnerable keyword) or vice versa.
  public:
   using base = ActionBase< DragEnemyAction, actions::ActionType< ActionLabel::DRAG_ENEMY > >;
   using base::base;

   DragEnemyAction(Team team, bool to_bf, size_t from, size_t to) noexcept
       : ActionBase(team), m_to_bf(to_bf), m_from(from), m_to(to)
   {
   }
   [[nodiscard]] inline auto to_bf() const { return m_to_bf; }
   [[nodiscard]] inline auto from() const { return m_from; }
   [[nodiscard]] inline auto to() const { return m_to; }

   bool execute_impl(State& state);

  private:
   bool m_to_bf;
   size_t m_from;
   size_t m_to;
};

/**
 * Action for deciding which cards to m_replace in the initial draw
 */
class MulliganAction:
    public ActionBase< MulliganAction, actions::ActionType< ActionLabel::MULLIGAN > > {
  public:
   using base = ActionBase< MulliganAction, actions::ActionType< ActionLabel::MULLIGAN > >;
   using base::base;

   explicit MulliganAction(Team team, std::vector< bool > replace)
       : ActionBase(team), m_replace(std::move(replace))
   {
   }
   [[nodiscard]] inline auto replace_decisions() const { return m_replace; }
   bool execute_impl(State& state);

  private:
   // the positions on the battlefield the units take
   std::vector< bool > m_replace;
};

class TargetingAction:
    public ActionBase< TargetingAction, actions::ActionType< ActionLabel::TARGETING > > {
  public:
   using base = ActionBase< TargetingAction, actions::ActionType< ActionLabel::TARGETING > >;
   using base::base;

   explicit TargetingAction(Team team, std::vector< sptr< Targetable > > targets)
       : ActionBase(team), m_targets(std::move(targets))
   {
   }
   [[nodiscard]] inline auto targets() const { return m_targets; }

   bool execute_impl(State& state);

  private:
   // the selected targets
   std::vector< sptr< Targetable > > m_targets;
};

class Action {
  public:
   using ActionVariant = std::variant<
      AcceptAction,
      CancelAction,
      ChoiceAction,
      DragEnemyAction,
      MulliganAction,
      PassAction,
      PlaceSpellAction,
      PlaceUnitAction,
      PlayAction,
      TargetingAction >;

   explicit Action(ActionVariant action) noexcept : m_action_detail(std::move(action)) {}
//   Action(const Action& action) noexcept = default;
//   Action(Action&& action) noexcept = default;
//   ~Action() = default;
//   Action& operator=(const Action& action) noexcept = default;
//   Action& operator=(Action&& action) noexcept = default;

   template < typename DetailType >
   [[nodiscard]] inline auto detail() const
   {
      return std::get< DetailType >(m_action_detail);
   }
   // default for not providing any template type and therefore simply getting the entire variant
   [[nodiscard]] inline auto detail() const { return m_action_detail; }

   [[nodiscard]] inline auto team() const
   {
      return std::visit([](const auto& action) { return action.team(); }, m_action_detail);
   }
   [[nodiscard]] constexpr inline auto label() const
   {
      return std::visit([](const auto& action) { return action.label; }, m_action_detail);
   }
   [[nodiscard]] inline bool execute(State& state)
   {
      return std::visit([&](auto& action) { return action.execute(state); }, m_action_detail);
   }
   [[nodiscard]] bool is_accept() const { return label() == ActionLabel::ACCEPT; }
   [[nodiscard]] bool is_cancellation() const { return label() == ActionLabel::CANCEL; }
   [[nodiscard]] bool is_choice() const { return label() == ActionLabel::CHOICE; }
   [[nodiscard]] bool is_dragging_enemy() const { return label() == ActionLabel::DRAG_ENEMY; }
   [[nodiscard]] bool is_mulligan() const { return label() == ActionLabel::MULLIGAN; }
   [[nodiscard]] bool is_pass() const { return label() == ActionLabel::PASS; }
   [[nodiscard]] bool is_placing_spell() const { return label() == ActionLabel::PLACE_SPELL; }
   [[nodiscard]] bool is_placing_unit() const { return label() == ActionLabel::PLACE_UNIT; }
   [[nodiscard]] bool is_play() const { return label() == ActionLabel::PLAY; }
   [[nodiscard]] bool is_targeting() const { return label() == ActionLabel::TARGETING; }

  private:
   // the type of action performed
   ActionVariant m_action_detail;
};
}  // namespace actions
#endif  // LORAINE_ACTION_H
