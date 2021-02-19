

#ifndef LORAINE_ACTION_H
#define LORAINE_ACTION_H

#include <map>
#include <utility>

#include "cards/card.h"
#include "engine/rulesets.h"
#include "utils/types.h"

enum ActionType { ACCEPT, ATTACK, BLOCK, CANCEL, MOVE_UNIT, MOVE_SPELL, MULLIGAN, PASS, PLAY };

class Action {
   // the type of action performed
   const ActionType m_action_type;
   // the round in which this action takes place
   size_t m_round;
   // the player who performed the action
   Player m_player;

  public:
   virtual ~Action() = default;

   [[nodiscard]] inline auto get_action_type() const { return m_action_type; }
   [[nodiscard]] inline auto get_round() const { return m_round; }
   [[nodiscard]] inline auto get_player() const { return m_player; }

   Action(ActionType act_type, size_t round, Player player)
       : m_action_type(act_type), m_round(round), m_player(player)
   {
   }
};

/*
 * Action for passing
 */
class PassAction: public Action {
  public:
   PassAction(size_t round, Player player) : Action(ActionType::PASS, round, player) {}
};
/*
 * This is the action for accepting the spell/skill outcome of whatever
 * action/reaction the opponent has played.
 */
class AcceptAction: public Action {
  public:
   AcceptAction(size_t round, Player player) : Action(ActionType::ACCEPT, round, player) {}
};
/*
 * Action for playing a unit/spell/skill
 */
class PlayAction: public Action {
   // the actual card that was played
   sptr< Card > card_played;
   std::optional< size_t > replace_idx;

  public:
   PlayAction(
      size_t round,
      Player player,
      sptr< Card > card_played,
      std::optional< size_t > replace_idx = {})
       : Action(ActionType::PLAY, round, player),
         card_played(std::move(card_played)),
         replace_idx(replace_idx)
   {
   }
   [[nodiscard]] inline auto get_card_played() const { return card_played; }
   [[nodiscard]] inline auto get_replace_idx() const { return replace_idx; }
};

class MoveSpellAction: public Action {
   sptr< Spell > m_spell;
   bool m_to_stack;

  public:
   MoveSpellAction(size_t round, Player player, sptr< Spell > spell, bool to_stack)
       : Action(MOVE_SPELL, round, player), m_spell(std::move(spell)), m_to_stack(to_stack)
   {
   }
   [[nodiscard]] inline auto get_spell() const { return m_spell; }
   [[nodiscard]] inline auto towards_stack() const { return m_to_stack; }
};

class MoveUnitAction: public Action {
   // the positions on the battlefield the units from the camp take.
   // One has a vector naming the position the unit from the camp
   // (where it holds the position of its current index in the source
   // container) onto the next position in the target container.
   bool m_to_bf;
   std::vector< size_t > m_indices_vec;
   std::map< size_t, size_t > m_opp_indices_map;

  public:
   MoveUnitAction(
      size_t round,
      Player player,
      bool to_bf,
      std::vector< size_t > indices_vec,
      std::map< size_t, size_t > opp_indices_map)
       : Action(MOVE_UNIT, round, player),
         m_to_bf(to_bf),
         m_indices_vec(std::move(indices_vec)),
         m_opp_indices_map(std::move(opp_indices_map))
   {
   }
   [[nodiscard]] inline auto towards_bf() const { return m_to_bf; }
   [[nodiscard]] inline auto get_indices_vec() const { return m_indices_vec; }
   [[nodiscard]] inline auto get_opp_indices_map() const { return m_opp_indices_map; }
};
/*
 * Action for declaring an attack
 */
class AttackAction: public Action {
   std::optional< std::vector< sptr< Spell > > > m_potential_spells;

  public:
   AttackAction(
      size_t round,
      Player player,
      std::optional< std::vector< sptr< Spell > > > potential_spells = {})
       : Action(ActionType::ATTACK, round, player), m_potential_spells(potential_spells)
   {
   }
   [[nodiscard]] inline auto get_potential_spells() const { return m_potential_spells; }
};

/*
 * Action for declaring block
 */
class BlockAction: public Action {
   std::optional< std::vector< sptr< Spell > > > m_potential_spells;

  public:
   BlockAction(
      size_t round, Player player, std::optional< std::vector< sptr< Spell > > > potential_spells)
       : Action(ActionType::BLOCK, round, player), m_potential_spells(potential_spells)
   {
   }
   [[nodiscard]] inline auto get_potential_spells() const { return m_potential_spells; }
};

/*
 * Action for deciding which cards to replace in the initial draw
 */
class MulliganAction: public Action {
   // the positions on the battlefield the units take
   std::array< bool, INITIAL_HAND_SIZE > replace;

  public:
   MulliganAction(size_t round, Player player, std::array< bool, INITIAL_HAND_SIZE > replace)
       : Action(ActionType::MULLIGAN, round, player), replace(replace)
   {
   }
   [[nodiscard]] inline auto get_replace_decisions() const { return replace; }
};


class CancelAction: public Action {
   CancelAction(size_t round, Player player)
   : Action(ActionType::CANCEL, round, player)
      {
      }
};
#endif  // LORAINE_ACTION_H