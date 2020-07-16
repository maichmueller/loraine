

#ifndef LORAINE_ACTION_H
#define LORAINE_ACTION_H

#include <cards/card.h>

#include <map>
#include <utility>

#include "rulesets.h"
#include "types.h"

enum ActionType { PASS, PLAY, ATTACK, BLOCK, ROUND_END, ACCEPT, MULLIGAN };

class Action {
   // the type of action performed
   const ActionType m_action_type;
   size_t m_round;
   PLAYER m_player;

  public:
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
   Action(ActionType act_type, size_t round, PLAYER player)
       : m_action_type(act_type), m_round(round), m_player(player)
   {
   }
};

/*
 * Action for passing
 */
class PassAction: public Action {
  public:
   PassAction(size_t round, PLAYER player)
       : Action(ActionType::PASS, round, player)
   {
   }
};

/*
 * This is the action for accepting the spell/skill outcome of whatever
 * action/reaction the opponent has played.
 */
class AcceptAction: public Action {
  public:
   AcceptAction(size_t round, PLAYER player)
       : Action(ActionType::ACCEPT, round, player)
   {
   }
};
class RoundEndAction: public Action {
  public:
   RoundEndAction(size_t round, PLAYER player)
       : Action(ActionType::ROUND_END, round, player)
   {
   }
};

/*
 * Action for playing a unit/spell/skill
 */
class PlayAction: public Action {
   // the actual card that was played
   sptr< Card > card_played;

  public:
   PlayAction(
      size_t round,
      PLAYER player,
      sptr< Card > card_played)
       : Action(ActionType::PLAY, round, player),
         card_played(std::move(card_played))
   {
   }
   [[nodiscard]] auto get_action_data() const
   {
      return _get_action_data(card_played);
   }
   [[nodiscard]] auto get_card_played() const
   {
      return card_played;
   }
};

/*
 * Action for declaring an attack
 */
class AttackAction: public Action {
   // the positions on the battlefield the units take
   std::vector< sptr< Card > > positions_to_cards;
  public:
   AttackAction(
      size_t round,
      PLAYER player,
      std::vector< sptr< Card > > positions_to_cards)
       : Action(ActionType::ATTACK, round, player),
         positions_to_cards(std::move(positions_to_cards))
   {
   }
   [[nodiscard]] auto get_action_data() const
   {
      return _get_action_data(positions_to_cards);
   }
   [[nodiscard]] auto get_positions_to_cards() const
   {
      return positions_to_cards;
   }
};

/*
 * Action for declaring block
 */
class BlockAction: public Action {
   // the positions on the battlefield the units take
   std::vector< sptr< Card > > positions_to_cards;

  public:
   BlockAction(
      size_t round,
      PLAYER player,
      std::vector< sptr< Card > > positions_to_cards)
       : Action(ActionType::BLOCK, round, player),
         positions_to_cards(std::move(positions_to_cards))
   {
   }
   [[nodiscard]] auto get_action_data() const
   {
      return _get_action_data(positions_to_cards);
   }
   [[nodiscard]] auto get_positions_to_cards() const
   {
      return positions_to_cards;
   }
};

/*
 * Action for deciding which cards to replace in the initial draw
 */
class MulliganAction: public Action {
   // the positions on the battlefield the units take
   std::array< bool, INITIAL_HAND_SIZE > replace;
  public:
   MulliganAction(
      size_t round,
      PLAYER player,
      std::array< bool, INITIAL_HAND_SIZE > replace)
       : Action(ActionType::MULLIGAN, round, player), replace(replace)
   {
   }
   [[nodiscard]] auto get_action_data() const
   {
      return _get_action_data(replace);
   }
   [[nodiscard]] auto get_replace_decisions() const
   {
      return replace;
   }
};

#endif  // LORAINE_ACTION_H
