

#ifndef LORAINE_ACTION_H
#define LORAINE_ACTION_H

#include <map>
#include <utility>
#include <variant>

#include <entt/entt.hpp>
#include "gamedefs.h"
#include "loraine/utils/utils.h"

class GameState;

namespace actions {
enum class ActionID {
   ACCEPT,
   CANCEL,
   CHOICE,
   DRAG_ENEMY,
   PLACE_UNIT,
   PLACE_SPELL,
   MULLIGAN,
   PLAY_FIELDCARD,
   PLAY_FINISH,
   PLAY_SPELL,
   PLAY_REQUEST,
   REPLACE_FIELDCARD,
   TARGETING
};

enum class ActionRank { PRIMITIVE = 0, MACRO };

template < ActionID action_id >
class ActionLabelType {
   constexpr static ActionRank _determine_rank()
   {
      constexpr std::array macro_ids{
         ActionID::PLAY_FIELDCARD, ActionID::PLAY_SPELL};

      if constexpr(not algo::any_of(
                      macro_ids, [&](const ActionID& label) {
                      return label == action_id;
                   })) {
         return ActionRank::MACRO;
      }
      return ActionRank::PRIMITIVE;
   }

  public:
   constexpr static ActionID id = action_id;
   constexpr static ActionRank rank = _determine_rank();
};

template < typename Derived, typename ActionT >
class ActionBase: public utils::CRTP< ActionBase, Derived, ActionT > {
  public:
   constexpr static ActionID id = ActionT::id;
   constexpr static ActionID rank = ActionT::rank;

   explicit ActionBase(Team team) noexcept : m_team(team) {}

   [[nodiscard]] inline auto team() const { return m_team; }
   inline void mark_complete() { m_completed = true; }

   inline bool execute(GameState& state) { return this->derived()->execute_impl(state); }

  private:
   Team m_team;
   bool m_completed = false;
};

/**
 * This is the action for accepting the outcome of whatever
 * the current state is. Counts as pass if nothing has been done
 */
class AcceptAction:
    public ActionBase< AcceptAction, ActionLabelType< ActionID::ACCEPT > > {
  public:
   using base = ActionBase< AcceptAction, ActionLabelType< ActionID::ACCEPT > >;
   using base::base;

   bool execute_impl(GameState& state);
};

/**
 * Action for playing a fieldcard
 */
class PlayRequestAction:
    public ActionBase< PlayRequestAction, ActionLabelType< ActionID::PLAY_REQUEST > > {
  public:
   using base = ActionBase<
      PlayRequestAction,
      ActionLabelType< ActionID::PLAY_REQUEST > >;

   explicit PlayRequestAction(Team team, size_t hand_index) noexcept
       : ActionBase(team), m_hand_index(hand_index)
   {
   }
   [[nodiscard]] inline auto index() const { return m_hand_index; }

   bool execute_impl(GameState& state);

  private:
   // the index of the spell in hand to play
   size_t m_hand_index;
};
/**
 * Action for finishing up playing a fieldcard
 */
class PlayFieldCardFinishAction:
    public ActionBase<
       PlayFieldCardFinishAction,
       ActionLabelType< ActionID::PLAY_FINISH > > {
  public:
   using base = ActionBase<
      PlayFieldCardFinishAction,
      ActionLabelType< ActionID::PLAY_FINISH > >;

   explicit PlayFieldCardFinishAction(
      Team team,
      std::optional< size_t > camp_index = std::nullopt) noexcept
       : ActionBase(team), m_camp_index(camp_index)
   {
   }
   [[nodiscard]] inline auto index() const { return m_camp_index; }

   bool execute_impl(GameState& state);

  private:
   std::optional< size_t > m_camp_index;
};
/**
 * Action for finishing up playing a fieldcard
 */
class PlaySpellFinishAction:
    public ActionBase<
       PlaySpellFinishAction,
       ActionLabelType< ActionID::PLAY_FINISH > > {
  public:
   using base = ActionBase<
      PlaySpellFinishAction,
      ActionLabelType< ActionID::PLAY_FINISH > >;

   explicit PlaySpellFinishAction(Team team, bool burst) noexcept
       : ActionBase(team), m_burst(burst)
   {
   }
   [[nodiscard]] auto burst() const { return m_burst; }

   bool execute_impl(GameState& state);

  private:
   bool m_burst;
};

/**
 * Action for playing a fieldcard
 */
class PlayAction:
    public ActionBase< PlayAction, ActionLabelType< ActionID::PLAY_FIELDCARD > > {
  public:
   using base = ActionBase< PlayAction, ActionLabelType< ActionID::PLAY_FIELDCARD > >;

   PlayAction(Team team, size_t hand_index) noexcept
       : ActionBase(team),
         m_hand_index(hand_index),
         m_target_index(std::nullopt),
         m_targets(std::nullopt)
   {
   }
   PlayAction(Team team, size_t hand_index, size_t target_index) noexcept
       : ActionBase(team),
         m_hand_index(hand_index),
         m_target_index(target_index),
         m_targets(std::nullopt)
   {
   }
   PlayAction(Team team, size_t hand_index, std::vector< entt::entity > targets) noexcept
       : ActionBase(team),
         m_hand_index(hand_index),
         m_target_index(std::nullopt),
         m_targets(std::move(targets))
   {
   }
   [[nodiscard]] inline auto index() const { return m_hand_index; }

   bool execute_impl(GameState& state);

  private:
   // the index of the spell in hand to play
   size_t m_hand_index;
   // the index in the camp to place the spell (should be set automatically, unless full)
   std::optional< size_t > m_target_index;
   std::optional< std::vector< entt::entity > > m_targets;
};

/**
 * Action for playing a fieldcard
 */
class ChoiceAction:
    public ActionBase< ChoiceAction, ActionLabelType< ActionID::CHOICE > > {
  public:
   using base = ActionBase< ChoiceAction, ActionLabelType< ActionID::CHOICE > >;

   explicit ChoiceAction(Team team, size_t n_choices, size_t choice) noexcept
       : ActionBase(team), m_nr_choices(n_choices), m_choice(choice)
   {
   }
   [[nodiscard]] inline auto n_choices() const { return m_nr_choices; }
   [[nodiscard]] inline auto choice() const { return m_choice; }

   bool execute_impl(GameState& state);

  private:
   size_t m_nr_choices;
   size_t m_choice;
};

class CancelAction:
    public ActionBase< CancelAction, ActionLabelType< ActionID::CANCEL > > {
  public:
   using base = ActionBase< CancelAction, ActionLabelType< ActionID::CANCEL > >;
   using base::base;

   bool execute_impl(GameState& state);
};

class PlaceSpellAction:
    public ActionBase< PlaceSpellAction, ActionLabelType< ActionID::PLACE_SPELL > > {
  public:
   using base = ActionBase<
      PlaceSpellAction,
      ActionLabelType< ActionID::PLACE_SPELL > >;

   PlaceSpellAction(Team team, size_t hand_index, bool to_stack) noexcept
       : ActionBase(team), m_hand_index(hand_index), m_to_stack(to_stack)
   {
   }
   [[nodiscard]] inline auto index() const { return m_hand_index; }
   [[nodiscard]] inline auto to_stack() const { return m_to_stack; }

   bool execute_impl(GameState& state);

  private:
   // the index of the spell in hand to place
   size_t m_hand_index;
   bool m_to_stack;
};

class PlaceUnitAction:
    public ActionBase< PlaceUnitAction, ActionLabelType< ActionID::PLACE_UNIT > > {
   // Action for moving units either from the camp (index-based) onto the battlefield or from the
   // battlefield onto the camp
  public:
   using base = ActionBase< PlaceUnitAction, ActionLabelType< ActionID::PLACE_UNIT > >;

   PlaceUnitAction(Team team, bool to_bf, std::vector< size_t > indices_vec)
       : ActionBase(team), m_to_bf(to_bf), m_indices_vec(std::move(indices_vec))
   {
   }
   [[nodiscard]] inline auto to_bf() const { return m_to_bf; }
   [[nodiscard]] inline auto indices_vec() const { return m_indices_vec; }

   bool execute_impl(GameState& state);

  private:
   bool m_to_bf;
   std::vector< size_t > m_indices_vec;
};

class DragEnemyAction:
    public ActionBase< DragEnemyAction, ActionLabelType< ActionID::DRAG_ENEMY > > {
   // Drags an opponent unit either from the camp onto the battlefield (e.g. via challenger or
   // vulnerable keyword) or vice versa.
  public:
   using base = ActionBase< DragEnemyAction, ActionLabelType< ActionID::DRAG_ENEMY > >;

   DragEnemyAction(Team team, bool to_bf, size_t from, size_t to) noexcept
       : ActionBase(team), m_to_bf(to_bf), m_from(from), m_to(to)
   {
   }
   [[nodiscard]] inline auto to_bf() const { return m_to_bf; }
   [[nodiscard]] inline auto from() const { return m_from; }
   [[nodiscard]] inline auto to() const { return m_to; }

   bool execute_impl(GameState& state);

  private:
   bool m_to_bf;
   size_t m_from;
   size_t m_to;
};

/**
 * Action for deciding which cards to m_replace in the initial draw
 */
class MulliganAction:
    public ActionBase< MulliganAction, ActionLabelType< ActionID::MULLIGAN > > {
  public:
   using base = ActionBase< MulliganAction, ActionLabelType< ActionID::MULLIGAN > >;

   explicit MulliganAction(Team team, std::vector< bool > replace)
       : ActionBase(team), m_replace(std::move(replace))
   {
   }
   [[nodiscard]] inline auto replace_decisions() const { return m_replace; }
   bool execute_impl(GameState& state);

  private:
   // the positions on the battlefield the units take
   std::vector< bool > m_replace;
};

class TargetingAction:
    public ActionBase< TargetingAction, ActionLabelType< ActionID::TARGETING > > {
  public:
   using base = ActionBase< TargetingAction, ActionLabelType< ActionID::TARGETING > >;

   explicit TargetingAction(Team team, std::vector< entt::entity > targets)
       : ActionBase(team), m_targets(std::move(targets))
   {
   }
   [[nodiscard]] inline auto targets() const { return m_targets; }

   bool execute_impl(GameState& state);

  private:
   // the selected targets
   std::vector< entt::entity > m_targets;
};

class ReplacingAction:
    public ActionBase<
       ReplacingAction,
       ActionLabelType< ActionID::REPLACE_FIELDCARD > > {
  public:
   using base = ActionBase<
      ReplacingAction,
      ActionLabelType< ActionID::REPLACE_FIELDCARD > >;

   explicit ReplacingAction(Team team, size_t replace_index)
       : ActionBase(team), m_replace_index(replace_index)
   {
   }
   [[nodiscard]] inline auto index() const { return m_replace_index; }

   bool execute_impl(GameState& state);

  private:
   // the selected targets
   size_t m_replace_index;
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
      PlayAction,
      PlayRequestAction,
      PlayFieldCardFinishAction,
      PlaySpellFinishAction,
      TargetingAction >;
  public:
   explicit Action(ActionVariant action) noexcept : m_action_detail(std::move(action)) {}

   template < typename DetailType >
   [[nodiscard]] inline auto& detail() const
   {
      return std::get< DetailType >(m_action_detail);
   }
   // default for not providing any template type and therefore simply getting the entire variant
   [[nodiscard]] inline auto& detail() const { return m_action_detail; }

   [[nodiscard]] inline auto team() const
   {
      return std::visit([](const auto& action) { return action.team(); }, m_action_detail);
   }
   [[nodiscard]] constexpr inline auto id() const
   {
      return std::visit([](const auto& action) { return action.id; }, m_action_detail);
   }
   inline bool execute(GameState& state)
   {
      return std::visit([&](auto& action) { return action.execute(state); }, m_action_detail);
   }
   [[nodiscard]] bool is_accept() const { return id() == ActionID::ACCEPT; }
   [[nodiscard]] bool is_cancellation() const { return id() == ActionID::CANCEL; }
   [[nodiscard]] bool is_choice() const { return id() == ActionID::CHOICE; }
   [[nodiscard]] bool is_dragging_enemy() const { return id() == ActionID::DRAG_ENEMY; }
   [[nodiscard]] bool is_mulligan() const { return id() == ActionID::MULLIGAN; }
   [[nodiscard]] bool is_placing_spell() const { return id() == ActionID::PLACE_SPELL; }
   [[nodiscard]] bool is_placing_unit() const { return id() == ActionID::PLACE_UNIT; }
   [[nodiscard]] bool is_play_finish() const { return id() == ActionID::PLAY_FINISH; }
   [[nodiscard]] bool is_play_request() const { return id() == ActionID::PLAY_REQUEST; }
   [[nodiscard]] bool is_targeting() const { return id() == ActionID::TARGETING; }

  private:
   // the type of action performed
   ActionVariant m_action_detail;
};
}  // namespace actions
#endif  // LORAINE_ACTION_H
