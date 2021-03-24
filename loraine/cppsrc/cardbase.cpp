
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
