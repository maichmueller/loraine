

#ifndef LORAINE_ACTION_H
#define LORAINE_ACTION_H

#include <map>
#include <utility>

#include "cards/card.h"
#include "core/gamedefs.h"
#include "utils/types.h"

enum class ActionType {
   ACCEPT,
   ATTACK,
   BLOCK,
   CANCEL,
   MOVE_UNIT,
   MOVE_SPELL,
   MULLIGAN,
   PASS,
   PLAY
};

class Action {
  public:
   Action(ActionType act_type) : m_action_type(act_type) {}
   virtual ~Action() = default;

   [[nodiscard]] inline auto action_type() const { return m_action_type; }

  private:
   // the type of action performed
   const ActionType m_action_type;
};

/**
 * Action for passing
 */
class PassAction: public Action {
  public:
   PassAction() : Action(ActionType::PASS) {}
};
/**
 * This is the action for accepting the outcome of whatever
 * the current state is.
 */
class AcceptAction: public Action {
  public:
   AcceptAction() : Action(ActionType::ACCEPT) {}
};
/**
 * Action for playing a fieldcard/spell
 */
class PlayAction: public Action {
  public:
   explicit PlayAction(sptr< Card > card_played, std::optional< size_t > replace_idx = {})
       : Action(ActionType::PLAY),
         m_card_played(std::move(card_played)),
         m_replacements(replace_idx)
   {
   }
   [[nodiscard]] inline auto card_played() const { return m_card_played; }
   [[nodiscard]] inline auto replacements() const { return m_replacements; }

  private:
   // the actual card that was played
   sptr< Card > m_card_played;
   std::optional< size_t > m_replacements;
};

class MoveSpellAction: public Action {
  public:
   MoveSpellAction(sptr< Spell > spell, bool to_stack)
       : Action(ActionType::MOVE_SPELL), m_spell(std::move(spell)), m_to_stack(to_stack)
   {
   }
   [[nodiscard]] inline auto spell() const { return m_spell; }
   [[nodiscard]] inline auto to_stack() const { return m_to_stack; }

  private:
   sptr< Spell > m_spell;
   bool m_to_stack;
};

class MoveUnitAction: public Action {
   // the positions on the battlefield the units from the camp take.
   // One has a vector naming the position the common from the camp
   // (where it holds the position of its current index in the source
   // ContainerType) onto the next position in the target ContainerType.
   bool m_to_bf;
   std::vector< size_t > m_indices_vec;
   std::map< size_t, size_t > m_opp_indices_map;

  public:
   MoveUnitAction(
      bool to_bf, std::vector< size_t > indices_vec, std::map< size_t, size_t > opp_indices_map)
       : Action(ActionType::MOVE_UNIT),
         m_to_bf(to_bf),
         m_indices_vec(std::move(indices_vec)),
         m_opp_indices_map(std::move(opp_indices_map))
   {
   }
   [[nodiscard]] inline auto to_bf() const { return m_to_bf; }
   [[nodiscard]] inline auto get_indices_vec() const { return m_indices_vec; }
   [[nodiscard]] inline auto get_opp_indices_map() const { return m_opp_indices_map; }
};
/**
 * Action for declaring an attack
 */
class AttackAction: public Action {
  public:
   AttackAction() : Action(ActionType::ATTACK) {}
};

/**
 * Action for declaring block
 */
class BlockAction: public Action {
  public:
   BlockAction() : Action(ActionType::BLOCK) {}
};

/**
 * Action for deciding which cards to replace in the initial draw
 */
class MulliganAction: public Action {
  public:
   explicit MulliganAction(std::vector< bool > replace)
       : Action(ActionType::MULLIGAN), replace(std::move(replace))
   {
   }
   [[nodiscard]] inline auto replace_decisions() const { return replace; }

  private:
   // the positions on the battlefield the units take
   std::vector< bool > replace;
};

#endif  // LORAINE_ACTION_H
