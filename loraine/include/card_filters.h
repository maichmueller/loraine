
#ifndef LORAINE_CARD_FILTERS_H
#define LORAINE_CARD_FILTERS_H

#include <utility>

#include "cards/card_types/card.h"

inline bool filter_trivial(const sptr< Card >& card)
{
   return true;
}
inline bool filter_noone(const sptr< Card >& card)
{
   return false;
}
inline bool filter_ally(const sptr< Card >& card, Team ally)
{
   return card->get_mutable_attrs().owner == ally;
}
inline bool filter_enemy(const sptr< Card >& card, Team ally)
{
   return card->get_mutable_attrs().owner != ally;
}
inline bool filter_health_less(const sptr< Card >& card, size_t health)
{
   return to_unit(card)->get_health() < health;
}
inline bool filter_health_greater(const sptr< Card >& card, size_t health)
{
   return to_unit(card)->get_health() > health;
}
inline bool filter_health_equals(const sptr< Card >& card, size_t health)
{
   return to_unit(card)->get_health() == health;
}
inline bool filter_power_less(const sptr< Card >& card, size_t power)
{
   return to_unit(card)->get_power() < power;
}
inline bool filter_power_greater(const sptr< Card >& card, size_t power)
{
   return to_unit(card)->get_power() > power;
}
inline bool filter_power_equals(const sptr< Card >& card, size_t power)
{
   return to_unit(card)->get_power() == power;
}
inline bool filter_cost_less(const sptr< Card >& card, size_t cost)
{
   return card->get_mana_cost() < cost;
}
inline bool filter_cost_greater(const sptr< Card >& card, size_t cost)
{
   return card->get_mana_cost() > cost;
}
inline bool filter_is_unit(const sptr< Card >& card)
{
   return card->is_unit();
}
inline bool filter_is_spell(const sptr< Card >& card)
{
   return card->is_spell();
}
inline bool filter_is_landmark(const sptr< Card >& card)
{
   return card->is_landmark();
}
inline bool filter_is_follower(const sptr< Card >& card)
{
   return card->is_follower();
}
inline bool filter_is_champion(const sptr< Card >& card)
{
   return card->is_champion();
}
inline bool filter_is_damaged(const sptr< Card >& card)
{
   return to_unit(card)->get_damage() > 0;
}
inline bool filter_is_stunned(const sptr< Card >& card)
{
   return card->has_keyword(Keyword::STUN);
}
inline bool filter_region(const sptr< Card >& card, Region region)
{
   return card->get_region() == region;
}
inline bool filter_group(const sptr< Card >& card, Group group)
{
   return card->get_group() == group;
}
inline bool filter_id(const sptr< Card >& card, const char* id)
{
   return card->get_id() == id;
}

#endif  // LORAINE_CARD_FILTERS_H
