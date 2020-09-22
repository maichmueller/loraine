

#ifndef LORAINE_ACTION_H
#define LORAINE_ACTION_H

#include <cards/card.h>

#include <map>
#include <utility>

#include "rulesets.h"
#include "types.h"

enum ActionType { PASS, PLAY, ATTACK, BLOCK, ACCEPT, MULLIGAN };

class AnyAction {
   // the type of action performed
   const ActionType m_action_type;
   size_t m_round;
   PLAYER m_player;

  public:
   virtual ~AnyAction() = default;
   /*
    * The base class method to return all base member variables
    */
   [[nodiscard]] auto get_action_data() const
   {
      return std::tuple{m_player, m_round, m_action_type};
   }
   [[nodiscard]] auto get_action_type() const { return m_action_type; }
   [[nodiscard]] auto get_round() const { return m_round; }
   [[nodiscard]] auto get_player() const { return m_player; }

  protected:
   /*
    * This is a protected stack of methods to allow the child classes to simply
    * call _get_action_data() in their own public get_action_data() methods
    * without needing to constantly copy over arguments from the base class into
    * the method call. This will be done automatically by the methods below!
    */
   template < typename Tuple, size_t... I, typename... Args >
   [[nodiscard]] auto _get_action_data_base(
      Tuple t, std::index_sequence< I... >, Args... args) const
   {
      return std::tuple{std::get< I >(t)..., args...};
   }
   template < typename Tuple, typename... Args >
   auto _get_action_data_base_applied(Tuple t, Args... args) const
   {
      static constexpr auto size = std::tuple_size< Tuple >::value;
      return _get_action_data_base(
         t, std::make_index_sequence< size >{}, args...);
   }
   // This is now the protected method to call when overriding the child's
   // method get_action_data() in the form of
   // auto get_action_data() {
   //    return _get_action_data(child_member_variable1,
   //       child_member_variable2, ...);
   // }
   template < typename... Args >
   [[nodiscard]] auto _get_action_data(Args... args) const
   {
      return _get_action_data_base_applied(get_action_data(), args...);
   }

   // protected constructor
   AnyAction(ActionType act_type, size_t round, PLAYER player)
       : m_action_type(act_type), m_round(round), m_player(player)
   {
   }
};

/*
 * Action for passing
 */
class PassAction: public AnyAction {
  public:
   PassAction(size_t round, PLAYER player)
       : AnyAction(ActionType::PASS, round, player)
   {
   }
};

/*
 * This is the action for accepting the spell/skill outcome of whatever
 * action/reaction the opponent has played.
 */
class AcceptAction: public AnyAction {
  public:
   AcceptAction(size_t round, PLAYER player)
       : AnyAction(ActionType::ACCEPT, round, player)
   {
   }
};
/*
 * Action for playing a unit/spell/skill
 */
class PlayAction: public AnyAction {
   // the actual card that was played
   sptr< Card > card_played;

  public:
   PlayAction(size_t round, PLAYER player, sptr< Card > card_played)
       : AnyAction(ActionType::PLAY, round, player),
         card_played(std::move(card_played))
   {
   }
   [[nodiscard]] auto get_action_data() const
   {
      return _get_action_data(card_played);
   }
   [[nodiscard]] auto get_card_played() const { return card_played; }
};

/*
 * Action for declaring an attack
 */
class AttackAction: public AnyAction {
   // the positions on the battlefield the units from the backrow take.
   // One has a vector naming the position the unit from the backrow
   // (where it holds the position of its current index in the backrow
   // container) onto the position specified in the vector.
   std::vector< size_t > camp_to_field_vec;
   std::map< size_t, size_t > opp_camp_to_field_map;
   std::optional< std::vector< sptr< Spell > > > potential_spells;

  public:
   AttackAction(
      size_t round,
      PLAYER player,
      std::vector< size_t > camp_to_field_vec,
      std::map< size_t, size_t > opp_camp_to_field_map,
      std::optional< std::vector< sptr< Spell > > > potential_spells)
       : AnyAction(ActionType::ATTACK, round, player),
         camp_to_field_vec(std::move(camp_to_field_vec)),
         opp_camp_to_field_map(std::move(opp_camp_to_field_map)),
         potential_spells(std::move(potential_spells))
   {
   }
   [[nodiscard]] auto get_action_data() const
   {
      return _get_action_data(
         camp_to_field_vec, opp_camp_to_field_map, potential_spells);
   }
   [[nodiscard]] auto get_positions_to_cards() const
   {
      return camp_to_field_vec;
   }
   [[nodiscard]] auto get_opp_positions_to_cards() const
   {
      return opp_camp_to_field_map;
   }
};

/*
 * Action for declaring block
 */
class BlockAction: public AnyAction {
   // the positions on the battlefield the units from the backrow take.
   // One has a vector naming the position the unit from the backrow
   // (where it holds the position of its current index in the backrow
   // container) onto the position specified in the vector.
   std::vector< size_t > back_to_field_vec;
   std::optional< std::vector< sptr< Spell > > > potential_spells;

  public:
   BlockAction(
      size_t round,
      PLAYER player,
      std::vector< size_t > back_to_field_vec,
      std::optional< std::vector< sptr< Spell > > > potential_spells)
       : AnyAction(ActionType::BLOCK, round, player),
         back_to_field_vec(std::move(back_to_field_vec)),
         potential_spells(std::move(potential_spells))
   {
   }
   [[nodiscard]] auto get_action_data() const
   {
      return _get_action_data(back_to_field_vec, potential_spells);
   }
   [[nodiscard]] auto get_positions_to_cards() const
   {
      return back_to_field_vec;
   }
};

/*
 * Action for deciding which cards to replace in the initial draw
 */
class MulliganAction: public AnyAction {
   // the positions on the battlefield the units take
   std::array< bool, INITIAL_HAND_SIZE > replace;

  public:
   MulliganAction(
      size_t round,
      PLAYER player,
      std::array< bool, INITIAL_HAND_SIZE > replace)
       : AnyAction(ActionType::MULLIGAN, round, player), replace(replace)
   {
   }
   [[nodiscard]] auto get_action_data() const
   {
      return _get_action_data(replace);
   }
   [[nodiscard]] auto get_replace_decisions() const { return replace; }
};

#endif  // LORAINE_ACTION_H
