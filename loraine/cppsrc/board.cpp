

#include "board.h"

#include "utils.h"

void Board::move_to_battlefield(
   const std::map< size_t, size_t >& units, Player player)
{
   auto& battlefield = get_battlefield(player);
   auto& camp = get_camp(player);
   for(auto camp_pos_to_field_pos : units) {
      auto camp_pos = camp_pos_to_field_pos.first;
      auto field_pos = camp_pos_to_field_pos.second;
      auto& unit = camp.at(camp_pos);
      battlefield.at(field_pos) = unit;
   }
}
void Board::move_to_battlefield(
   const std::vector< size_t >& field_positions, Player player)
{
   std::map< size_t, size_t > unit_map;
   for(size_t i = 0; i < field_positions.size(); ++i) {
      size_t field_pos = field_positions.at(i);
      unit_map.emplace(i, field_pos);
   }
   move_to_battlefield(unit_map, player);
}
std::pair< bool, Board::Camp::iterator > Board::find_in_camp(
   const sptr< Unit >& unit)
{
   auto& camp = m_camp.at(unit->get_owner());
   auto end = camp.end();
   auto found = std::find(camp.begin(), camp.end(), *unit);
   return std::pair< bool, Camp::iterator >(found == end, found);
}
std::pair< bool, Board::Battlefield::iterator > Board::find_on_battlefield(
   const sptr< Unit >& unit)
{
   auto& battlefield = m_battlefield.at(unit->get_owner());
   auto* end = battlefield.end();
   auto* found = std::find(battlefield.begin(), end, *unit);
   return std::pair< bool, Battlefield::iterator >(found == end, found);
}
void Board::remove_dead_units(const std::vector< sptr< Unit > >& units)
{
   for(const auto& unit : units) {
      if(auto [found, unit_iter] = find_in_camp(unit); found) {
         m_camp[unit->get_owner()].erase(unit_iter);
      } else if(auto [found, unit_iter] = find_on_battlefield(unit); found) {
         unit_iter->reset();
      } else {
         // TODO: Log that no unit was found?
      }
   }
}
void Board::remove_dead_units(
   Player player, std::vector< size_t > indices, bool in_camp)
{
   std::set< size_t > sorted_indices(indices.begin(), indices.end());
   size_t reduction = 0;
   if(in_camp) {
      auto camp = m_camp[player];
      for(auto idx : sorted_indices) {
         camp.erase(camp.begin() + (idx - reduction));
         reduction += 1;
      }
   } else {
      auto bf = m_battlefield[player];
      for(auto idx : sorted_indices) {
         bf.at(idx).reset();
      }
   }
}

size_t Board::count_units(
   Player player,
   bool in_camp,
   const std::function< bool(const sptr< Unit >&) >& filter) const
{
   size_t sum = 0;
   if(in_camp) {
      for(const auto& pot_unit : m_camp.at(player)) {
         if(filter(pot_unit)) {
            sum += 1;
         } else {
            break;
         }
      }
   } else {
      for(const auto& pot_unit : m_battlefield.at(player)) {
         if(pot_unit.has_value() && filter(pot_unit.value())) {
            sum += 1;
         } else {
            break;
         }
      }
   }
   return sum;
}
size_t Board::count_units(Player player, bool in_camp) const
{
   return in_camp ? m_camp[player].size()
                  : count_units(player, false, [](const auto& /*unused*/) {
                       return true;
                    });
}
