

#include "board.h"

void Board::move_to_battlefield(const std::map< size_t, size_t >& units, PLAYER player)
{
   auto& battlefield = get_battlefield(player);
   auto& backrow = get_camp(player);
   for(auto camp_pos_to_field_pos : units) {
      auto camp_pos = camp_pos_to_field_pos.first;
      auto field_pos = camp_pos_to_field_pos.second;
      auto& unit = backrow.at(camp_pos);
      backrow.at(camp_pos).reset();
      battlefield.at(field_pos) = unit;
   }
}
void Board::move_to_battlefield(
   const std::vector< size_t >& field_positions, PLAYER player)
{
   std::map< size_t, size_t > unit_map;
   for(size_t i = 0; i < field_positions.size(); ++i) {
      size_t field_pos = field_positions.at(i);
      unit_map.emplace(i, field_pos);
   }
   move_to_battlefield(unit_map, player);
}
void Board::reorganize_camp(PLAYER player, size_t start_from_idx) {
   auto & camp = m_camp[player];
   for(size_t idx = start_from_idx; idx < CAMP_SIZE; ++idx) {

   }
}
