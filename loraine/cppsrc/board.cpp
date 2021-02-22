

#include "engine/board.h"

#include "cards/card.h"
#include "utils/utils.h"

size_t Board::count_units(
   Team team, bool in_camp, const std::function< bool(const sptr< FieldCard >&) >& filter) const
{
   size_t sum = 0;
   if(in_camp) {
      for(const auto& pot_unit : m_camp.at(team)) {
         if(pot_unit->is_unit() && filter(to_unit(pot_unit))) {
            sum += 1;
         } else {
            break;
         }
      }
   } else {
      for(const auto& pot_unit : m_battlefield.at(team)) {
         if(has_value(pot_unit) && filter(pot_unit)) {
            sum += 1;
         } else {
            break;
         }
      }
   }
   return sum;
}
size_t Board::count_units(Team team, bool in_camp) const
{
   return in_camp ? m_camp[team].size()
                  : count_units(team, false, [](const auto& /*unused*/) { return true; });
}

std::vector< sptr<Unit> > Board::camp_units(Team team) const
{
   std::vector< sptr< Unit > > units;
   for(const auto& card : m_camp[team]) {
      if(card->is_unit()) {
         units.emplace_back(to_unit(card));
      }
   }
   return units;
}
void Board::add_to_queue(const sptr< FieldCard >& card)
{
   m_camp_queue[card->mutables().owner].emplace(card);
}
void Board::add_to_queue(std::vector< sptr<FieldCard> >&& cards)
{
   for(auto&& card : cards) {
      m_camp_queue[card->mutables().owner].emplace(std::move(card));
   }
}
