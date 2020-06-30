

#ifndef LORAINE_ACTION_H
#define LORAINE_ACTION_H

#include <cards/card.h>

#include <map>

#include "types.h"

enum ActionType {
   PASS,
   PLAY,
   ATTACK,
   BLOCK,
   ROUND_START,
   ROUND_END,
   ACCEPT
};

class Action {
   /*
    * The base class method to return all base member variables
    */
   [[nodiscard]] auto get_action_data() const
   {
      return std::tuple{m_player, m_round, m_action_type};
   }

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
      return std::tuple{get_action_data(), args...};
   }

   // the type of action performed
   ActionType m_action_type;
   size_t m_round;
   PLAYER m_player;
};

/*
 * Action for passing
 */
class PassAction: public Action {
};

/*
 * This is the action for accepting the spell/skill outcome of whatever
 * action/reaction the opponent has played.
 */
class AcceptAction: public Action {
};
class RoundStartAction: public Action {
};
class RoundEndAction: public Action {
};

/*
 * Action for playing a unit/spell/skill
 */
class PlayAction: public Action {
   // the actual card that was played
   sptr< Card > card_played;
   // the potential skill it might have triggered
   sptr< Spell > skill_triggered;
};

/*
 * Action for declaring an attack
 */
class AttackAction: public Action {
   // the positions on the battlefield the units take
   std::map< size_t, sptr< Card > > positions_to_cards;
};

/*
 * Action for declaring block
 */
class BlockAction: public Action {
   // the positions on the battlefield the units take
   std::map< size_t, sptr< Card > > positions_to_cards;
};

#endif  // LORAINE_ACTION_H
