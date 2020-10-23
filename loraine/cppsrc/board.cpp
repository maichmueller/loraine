

#include "board.h"

#include "utils.h"

std::pair< bool, Board::Camp::iterator > Board::find_in_camp(const sptr< Unit >& unit)
{
   auto& battlefield = m_camp.at(unit->get_owner());
   auto end = battlefield.end();
   auto found = std::find(battlefield.begin(), end, unit);
   return {found != end, found};
}
std::pair< bool, Board::Battlefield::iterator > Board::find_on_battlefield(const sptr< Unit >& unit)
{
   auto& battlefield = m_battlefield.at(unit->get_owner());
   auto* end = battlefield.end();
   auto* found = std::find(battlefield.begin(), end, unit);
   return {found != end, found};
}
void Board::remove_units(const std::vector< sptr< Unit > >& units)
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

void Board::remove_units(Player player, std::vector< size_t > indices, bool in_camp)
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
   Player player, bool in_camp, const std::function< bool(const sptr< Unit >&) >& filter) const
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
                  : count_units(player, false, [](const auto& /*unused*/) { return true; });
}
