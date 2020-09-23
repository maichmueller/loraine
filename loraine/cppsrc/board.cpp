

#include "board.h"

#include "utils.h"

void Board::move_to_battlefield(
   const std::map< size_t, size_t >& units, PLAYER player)
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
   reorganize_camp(player);
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
void Board::reorganize_camp(PLAYER player, size_t start_from_idx)
{
   auto& camp = m_camp[player];
   std::array< size_t, CAMP_SIZE > indices_to_move_up;
   indices_to_move_up.fill(0);
   for(size_t idx = start_from_idx; idx < CAMP_SIZE; ++idx) {
      if(! camp[idx].has_value()) {
         for(size_t moveup_idx = idx + 1; moveup_idx < CAMP_SIZE;
             ++moveup_idx) {
            indices_to_move_up[moveup_idx] += 1;
         }
      }
   }
   for(size_t idx = start_from_idx; idx < CAMP_SIZE; ++idx) {
      if(auto positions_to_moveup = indices_to_move_up.at(idx);
         positions_to_moveup > 0) {
         camp.at(idx - positions_to_moveup) = camp.at(idx);
      }
   }
}
std::pair< bool, Board::Camp::iterator > Board::find_in_camp(
   const sptr< Unit >& unit)
{
   auto& camp = m_camp.at(unit->get_owner());
   auto end = camp.end();
   auto found = std::find_if(camp.begin(), camp.end(), [&](const auto& val) {
      if(val.has_value()) {
         if(*(val.value()) == *unit) {
            return val;
         }
      }
   });
   return std::pair< bool, Camp::iterator >(found == end, found);
}
std::pair< bool, Board::Battlefield::iterator > Board::find_on_battlefield(
   const sptr< Unit >& unit)
{
   auto& battlefield = m_battlefield.at(unit->get_owner());
   auto end = battlefield.end();
   auto found = std::find_if(battlefield.begin(), end, [&](const auto& val) {
      if(val.has_value()) {
         if(*(val.value()) == *unit) {
            return val;
         }
      }
   });
   return std::pair< bool, Battlefield::iterator >(found == end, found);
}
void Board::remove_dead_unit(const sptr< Unit >& unit)
{
   if(auto [found, opt_unit_iter] = find_in_camp(unit); found) {
      opt_unit_iter->reset();
   } else if(auto [found, opt_unit_iter] = find_on_battlefield(unit); found) {
      opt_unit_iter->reset();
   } else {
      // TODO: Log that no unit was found?
   }
}
void Board::retreat_to_camp(PLAYER player)
{
   size_t camp_units_count = count_camp(player, true);
   size_t bf_units_count = count_camp(player, false);
   size_t units_in_queue = m_camp_queue.size();
   auto& camp = m_camp.at(player);
   if(auto rem_slots = CAMP_SIZE - camp_units_count - bf_units_count;
      rem_slots > 0) {
      size_t curr_idx = camp_units_count - 1;
      auto& queue = m_camp_queue.at(player);
      for(size_t counter = 0; counter < std::min(units_in_queue, rem_slots);
          ++counter) {
         camp.at(curr_idx) = queue.back();
         queue.pop();
         curr_idx += 1;
      }
   }
   camp_units_count = count_camp(player, true);
   size_t curr_idx = camp_units_count - 1;
   for(auto& opt_unit : m_battlefield.at(player)) {
      if(opt_unit.has_value()) {
         std::swap(camp.at(curr_idx), opt_unit);
         curr_idx += 1;
      }
   }
}
size_t Board::count_camp(PLAYER player, bool in_camp)
{
   size_t sum = in_camp ? m_camp.size() : m_battlefield.size();
   for(auto& pot_unit :
       reverse(in_camp ? m_camp.at(player) : m_battlefield.at(player))) {
      if(! pot_unit.has_value()) {
         sum -= 1;
      } else {
         break;
      }
   }

   return sum;
}
