
#include <cards/types/cardbase.h>

#include <utility>

#include "cards/card.h"
#include "core/gamemode.h"
#include "grants/grant.h"
#include "utils/utils.h"

Card::Card(ConstState const_attrs, MutableState var_attrs)
    : m_immutables(std::move(const_attrs)), m_mutables(std::move(var_attrs))
{
}

void Card::remove_effect(events::EventLabel e_type, const EffectBase& effect)
{
   if(auto eff_vec_iter = std::find_if(
         m_mutables.effects.begin(),
         m_mutables.effects.end(),
         [&](const auto& val) { return val.first == e_type; });
      eff_vec_iter != m_mutables.effects.end()) {
      auto& eff_vec = (*eff_vec_iter).second;
      auto position = std::find_if(
         eff_vec.begin(), eff_vec.end(), [&](const auto& eff_ptr) { return (*eff_ptr) == effect; });
      if(position != eff_vec.end()) {
         if(eff_vec.size() == 1) {
            m_mutables.effects.erase(eff_vec_iter);
         } else {
            eff_vec.erase(position);
         }
      }
   }
}
void Card::add_effect(events::EventLabel e_type, sptr< EffectBase > effect)
{
   // if the key is already found in the m_effects map, delete the previous
   // effect. This essentially implies we overwrite preexisting m_effects
   if(m_mutables.effects.find(e_type) != m_mutables.effects.end()) {
      m_mutables.effects.erase(e_type);
   }
   m_mutables.effects[e_type].emplace_back(std::move(effect));
}
void Card::effects(events::EventLabel e_type, std::vector< sptr< EffectBase > > effects)
{
   auto& curr_vec = m_mutables.effects[e_type];
   for(auto&& eff : effects) {
      curr_vec.emplace_back(std::move(eff));
   }
}

std::vector< sptr< Grant > > Card::all_grants() const
{
   return mutables().grants_temp + mutables().grants;
}
void Card::store_grant(const sptr< Grant >& grant)
{
   if(grant->is_permanent()) {
      m_mutables.grants.emplace_back(grant);
   } else {
      m_mutables.grants_temp.emplace_back(grant);
   }
}
bool Card::has_effect(events::EventLabel e_type, const EffectBase& effect) const
{
   auto found_effects = m_mutables.effects.find(e_type);
   if(found_effects != m_mutables.effects.end()) {
      const auto& effects = found_effects->second;
      return std::find_if(
                effects.begin(),
                effects.end(),
                [&](const auto& eff_ptr) { return (*eff_ptr) == effect; })
             == effects.end();
   }
   return false;
}
Card::Card(const Card& card)
    : m_immutables(card.m_immutables),
      m_mutables(MutableState{
         card.m_mutables.owner,
         card.m_mutables.location,
         card.m_mutables.position,
         card.m_mutables.hidden,
         card.m_mutables.mana_cost_base,
         card.m_mutables.mana_cost_delta,
         card.m_mutables.keywords,
         _clone_effect_map(card.m_mutables.effects),
         card.m_mutables.play_toll->clone(),
         card.m_mutables.grants,
         card.m_mutables.grants_temp})
{
}
Card::EffectMap Card::_clone_effect_map(const Card::EffectMap& emap)
{
   EffectMap emap_new;
   for(const auto& [elabel, effect_container] : emap) {
      EffectMap::mapped_type new_entry;  // should be std::vector
      new_entry.reserve(effect_container.size());  // reserve the memory needed to clone
      std::transform(
         effect_container.begin(),
         effect_container.end(),
         std::back_inserter(new_entry),
         [](const auto& effect_ptr) { return effect_ptr->clone(); });
      emap_new[elabel] = new_entry;
   }
   return emap_new;
}
