

#ifndef LORAINE_ACTION_H
#define LORAINE_ACTION_H

#include <map>
#include <utility>

//#include "cards/card.h"
#include "core/gamedefs.h"
#include "core/targetable.h"
#include "utils/types.h"

class Card;
class FieldCard;
class Spell;

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

class Action {
  public:
   Action(ActionLabel act_type, Team team) noexcept : m_action_type(act_type), m_team(team) {}
   Action(const Action& action) noexcept = default;
   Action(Action&& action) noexcept = default;
   virtual ~Action() = default;
   Action& operator=(const Action& action) noexcept = delete;
   Action& operator=(Action&& action) noexcept = delete;

   [[nodiscard]] inline auto action_label() const { return m_action_type; }
   [[nodiscard]] inline auto team() const { return m_team; }

  private:
   // the type of action performed
   const ActionLabel m_action_type;
   const Team m_team;
};

/**
 * Action for passing
 */
class PassAction: public Action {
  public:
   explicit PassAction(Team team) noexcept : Action(ActionLabel::PASS, team) {}
};
/**
 * This is the action for accepting the outcome of whatever
 * the current state is.
 */
class AcceptAction: public Action {
  public:
   explicit AcceptAction(Team team) noexcept : Action(ActionLabel::ACCEPT, team) {}
};
/**
 * Action for playing a fieldcard
 */
class PlayAction: public Action {
  public:
   explicit PlayAction(Team team, sptr< FieldCard > card_played) noexcept
       : Action(ActionLabel::PLAY, team), m_card_played(std::move(card_played))
   {
   }
   [[nodiscard]] inline auto card_played() const { return m_card_played; }

  private:
   // the actual card that was played
   sptr< FieldCard > m_card_played;
};

/**
 * Action for playing a fieldcard
 */
class ChoiceAction: public Action {
  public:
   explicit ChoiceAction(Team team, size_t n_choices, size_t choice) noexcept
       : Action(ActionLabel::CHOICE, team), m_nr_choices(n_choices), m_choice(choice)
   {
   }
   [[nodiscard]] inline auto n_choices() const { return m_nr_choices; }
   [[nodiscard]] inline auto choice() const { return m_choice; }

  private:
   size_t m_nr_choices;
   size_t m_choice;
};

class CancelAction: public Action {
  public:
   explicit CancelAction(Team team) noexcept : Action(ActionLabel::CANCEL, team) {}
};

class PlaceSpellAction: public Action {
  public:
   PlaceSpellAction(Team team, sptr< Spell > spell, bool to_stack) noexcept
       : Action(ActionLabel::PLACE_SPELL, team), m_spell(std::move(spell)), m_to_stack(to_stack)
   {
   }
   [[nodiscard]] inline auto spell() const { return m_spell; }
   [[nodiscard]] inline auto to_stack() const { return m_to_stack; }

  private:
   sptr< Spell > m_spell;
   bool m_to_stack;
};

class PlaceUnitAction: public Action {
   // Action for moving units either from the camp (index-based) onto the battlefield or from the
   // battlefield onto the camp
   bool m_to_bf;
   std::vector< size_t > m_indices_vec;

  public:
   PlaceUnitAction(Team team, bool to_bf, std::vector< size_t > indices_vec)
       : Action(ActionLabel::PLACE_UNIT, team), m_to_bf(to_bf), m_indices_vec(std::move(indices_vec))
   {
   }
   [[nodiscard]] inline auto to_bf() const { return m_to_bf; }
   [[nodiscard]] inline auto get_indices_vec() const { return m_indices_vec; }
};

class DragEnemyAction: public Action {
   // Drags an opponent unit either from the camp onto the battlefield (e.g. via challenger or
   // vulnerable keyword) or vice versa.
   bool m_to_bf;
   size_t m_from;
   size_t m_to;

  public:
   DragEnemyAction(Team team, bool to_bf, size_t from, size_t to) noexcept
       : Action(ActionLabel::DRAG_ENEMY, team), m_to_bf(to_bf), m_from(from), m_to(to)
   {
   }
   [[nodiscard]] inline auto to_bf() const { return m_to_bf; }
   [[nodiscard]] inline auto from() const { return m_from; }
   [[nodiscard]] inline auto to() const { return m_to; }
};

/**
 * Action for deciding which cards to replace in the initial draw
 */
class MulliganAction: public Action {
  public:
   explicit MulliganAction(Team team, std::vector< bool > replace)
       : Action(ActionLabel::MULLIGAN, team), replace(std::move(replace))
   {
   }
   [[nodiscard]] inline auto replace_decisions() const { return replace; }

  private:
   // the positions on the battlefield the units take
   std::vector< bool > replace;
};

class TargetingAction: public Action {
  public:
   explicit TargetingAction(Team team, std::vector< sptr< Targetable > > targets)
       : Action(ActionLabel::TARGETING, team), m_targets(std::move(targets))
   {
   }
   [[nodiscard]] inline auto targets() const { return m_targets; }

  private:
   // the selected targets
   std::vector< sptr< Targetable > > m_targets;
};

#endif  // LORAINE_ACTION_H
