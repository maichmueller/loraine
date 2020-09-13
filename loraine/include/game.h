
#ifndef LORAINE_GAME_H
#define LORAINE_GAME_H

#include "agent.h"
#include "cards/card.h"
#include "rulesets.h"
#include "state.h"
#include "types.h"

class Game {
   sptr< State > m_state;
   SymArr< sptr<Agent> > m_agents;

   bool m_battle_mode = false;

   void _mulligan(
      const std::vector< sptr< Card > >& hand_blue,
      const std::vector< sptr< Card > >& hand_red);
   std::vector< sptr< Card > > _draw_initial_hand(
      PLAYER player);
   bool _do_action(const sptr< AnyAction >& action);
   /*
    * Start the next round
    */
   void _start_round();

   void _end_round();

   void _activate_battlemode(
      PLAYER attack_player, std::vector< size_t > positions);

   void _resolve_battle();

   void _resolve_spell_stack();

   void _nexus_hit(
      PLAYER attacked_nexus,
      PLAYER attacking_player,
      const sptr< size_t >& damage,
      std::shared_ptr< Card > responsible_card);

   int _strike_unit(
      const PLAYER& attacker,
      const PLAYER& defender,
      const std::shared_ptr< Unit >& unit_att,
      std::shared_ptr< Unit >& unit_def) const;

   void _kill_unit(PLAYER killer, const std::shared_ptr< Unit >& unit);

   int _deal_damage_to_unit(
      const PLAYER& player_of_unit,
      std::shared_ptr< Unit >& unit,
      const sptr< size_t >& damage) const;

  public:
   bool run_game();
};

#endif  // LORAINE_GAME_H
