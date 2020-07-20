

#include "board.h"

void Board::move_to_battlefield(std::vector< size_t > attackers, PLAYER player)
{
   auto & battlefield = get_battlefield(player);
   auto & backrow = get_backrow(player);
   size_t br_pos = 0;
   for (auto bf_pos : attackers) {
      auto & unit = backrow.at(br_pos);
      backrow.at(br_pos).reset();
      battlefield.at(bf_pos) = unit;
   }
}