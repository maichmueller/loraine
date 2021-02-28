
#include <cards/types/cardbase.h>

#include <utility>

#include "cards/card.h"
#include "gamemode.h"
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
      auto position = std::find(eff_vec.begin(), eff_vec.end(), effect);
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
bool Card::check_play_tribute(const State& state) const
{
   size_t total_mana = state.mana(m_mutables.owner);
   if(is_spell()) {
      total_mana += state.floating_mana(m_mutables.owner);
   }
   return mana_cost() <= total_mana && _check_play_condition(state);
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
   bool found = found_effects != m_mutables.effects.end();
   if(found) {
      const auto& effects = found_effects->second;
      return std::find(effects.begin(), effects.end(), effect) != effects.end();
   }
   return false;
}
