

#include "board.h"

#include "cards/card_types/all_card_types.h"
#include "utils.h"


size_t Board::count_units(
   Player player, bool in_camp, const std::function< bool(const sptr< Card >&) >& filter) const
{
   size_t sum = 0;
   if(in_camp) {
      for(const auto& pot_unit : m_camp.at(player)) {
         if(pot_unit->is_unit() && filter(to_unit(pot_unit))) {
            sum += 1;
         } else {
            break;
         }
      }
   } else {
      for(const auto& pot_unit : m_battlefield.at(player)) {
         if(has_value(pot_unit) && filter(pot_unit)) {
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

std::vector< sptr<Unit> > Board::get_camp_units(Player player) const
{
   std::vector< sptr< Unit > > units;
   for(const auto& card : m_camp[player]) {
      if(card->is_unit()) {
         units.emplace_back(to_unit(card));
      }
   }
   return units;
}
void Board::add_to_queue(const sptr< Card >& card)
{
   m_camp_queue[card->get_mutable_attrs().owner].emplace(card);
}
void Board::add_to_queue(std::vector< sptr<Card> >&& cards)
{
   for(auto&& card : cards) {
      m_camp_queue[card->get_mutable_attrs().owner].emplace(std::move(card));
   }
}
