#ifndef LORAINE_CARDS_H
#define LORAINE_CARDS_H

#include <entt/entt.hpp>
#include <string>

#include "loraine/core/components/all.h"

/**
 * Call this function from any of the actual card factories, to get the attributes already assigned.
 * Specifics of each card, such as effects, conditions, etc. are added in the respective specific
 * card factory.
 *
 * @param registry
 * @param code
 * @return
 */
template < size_t N >
entt::entity make_card(entt::registry& registry, const char code[N])
{
   auto card = registry.create();
   auto attrs = gallery::attributes(code);

   registry.emplace< CardAttributes >(
      card,
      attrs.name,
      attrs.code,
      attrs.description,
      attrs.lore,
      attrs.region,
      attrs.group,
      attrs.super_type,
      attrs.rarity,
      attrs.card_type,
      attrs.is_collectible,
      true);
}

#endif  // LORAINE_CARDS_H
