

#include "loraine/core/board.h"
#include "loraine/core/components/all.h"
#include "loraine/utils/utils.h"

std::vector< entt::entity > Board::camp_units(entt::registry& registry, Team team) const
{
   std::vector< entt::entity > units;
   for(const auto& card : m_camp[team]) {
      if(registry.get< CardAttributes >(card).card_type == CardType::UNIT) {
         units.emplace_back(card);
      }
   }
   return units;
}
void Board::add_to_camp_queue(entt::registry& registry, entt::entity card)
{
   m_camp_queue[registry.get< Team >(card)].emplace(card);
}
void Board::add_to_camp_queue(entt::registry& registry, std::vector< entt::entity >&& units)
{
   for(auto&& card : units) {
      m_camp_queue[registry.get< Team >(card)].emplace(std::move(card));
   }
}
void Board::add_to_bf_queue(entt::registry& registry, entt::entity unit)
{
   m_bf_queue[registry.get< Team >(unit)].emplace(unit);
}
void Board::add_to_bf_queue(entt::registry& registry, std::vector< entt::entity >&& units)
{
   for(auto&& unit : units) {
      m_bf_queue[registry.get< Team >(unit)].emplace(std::move(unit));
   }
}
void Board::add_to_bf(entt::registry& registry, entt::entity card, size_t idx)
{
   _fill_with_null(registry.get< Team >(card), idx);
   add_to_bf(registry, card);
}
void Board::add_to_bf(entt::registry& registry, entt::entity card)
{
   m_bf[registry.get< Team >(card)].emplace_back(card);
}
void Board::add_to_camp(entt::registry& registry, entt::entity card)
{
   m_camp[registry.get< Team >(card)].emplace_back(card);
}
