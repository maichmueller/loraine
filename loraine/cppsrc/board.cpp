

#include "core/board.h"

#include "cards/card.h"
#include "utils/utils.h"

size_t Board::count_units(
   Team team,
   bool in_camp,
   const std::function< bool(const sptr< FieldCard >&) >& filter) const
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
      for(const auto& pot_unit : m_bf.at(team)) {
         if(utils::has_value(pot_unit) && filter(pot_unit)) {
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

std::vector< sptr< Unit > > Board::camp_units(Team team) const
{
   std::vector< sptr< Unit > > units;
   for(const auto& card : m_camp[team]) {
      if(card->is_unit()) {
         units.emplace_back(to_unit(card));
      }
   }
   return units;
}
void Board::add_to_camp_queue(const sptr< FieldCard >& card)
{
   m_camp_queue[card->mutables().owner].emplace(card);
}
void Board::add_to_camp_queue(std::vector< sptr< FieldCard > >&& units)
{
   for(auto&& card : units) {
      m_camp_queue[card->mutables().owner].emplace(std::move(card));
   }
}
void Board::add_to_bf_queue(const sptr< Unit >& unit)
{
   m_bf_queue[unit->mutables().owner].emplace(unit);
}
void Board::add_to_bf_queue(std::vector< sptr< Unit > >&& units)
{
   for(auto&& unit : units) {
      m_bf_queue[unit->mutables().owner].emplace(std::move(unit));
   }
}
void Board::add_to_bf(const sptr< Unit >& card, size_t idx)
{
   _fill_with_nullptr(card->mutables().owner, idx);
   add_to_bf(card);
}
void Board::add_to_bf(const sptr< Unit >& card)
{
   m_bf[card->mutables().owner].emplace_back(card);
}
void Board::add_to_camp(const sptr< Unit >& card)
{
   m_camp[card->mutables().owner].emplace_back(card);
}

