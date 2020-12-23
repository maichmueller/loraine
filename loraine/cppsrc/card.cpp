
#include "cards/card_types/all_card_types.h"

#include <utility>

#include "cards/grant.h"
#include "game.h"
#include "utils.h"

Card::Card(
   ConstData const_attrs,
   MutableData var_attrs)
    : m_immutables(std::move(const_attrs)),
      m_mutables(std::move(var_attrs))
{
}

void Card::remove_effect(events::EventType e_type, const Effect& effect)
{
   if(auto eff_vec_iter = std::find_if(
         m_mutables.effects.begin(), m_mutables.effects.end(), [&](const auto& val) { return val.first == e_type; });
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
void Card::add_effect(events::EventType e_type, Effect effect)
{
   // if the key is already found in the effects map, delete the previous
   // effect. This essentially implies we overwrite preexisting effects
   if(m_mutables.effects.find(e_type) != m_mutables.effects.end()) {
      m_mutables.effects.erase(e_type);
   }
   m_mutables.effects[e_type].emplace_back(std::move(effect));
}
void Card::set_effect_vec(events::EventType e_type, std::vector< Effect > effects)
{
   auto& curr_vec = m_mutables.effects[e_type];
   for(auto&& eff : effects) {
      curr_vec.emplace_back(std::move(eff));
   }
}
bool Card::check_play_tribute(const Game& game) const
{
   auto state = game.get_state();
   size_t total_mana = state->get_mana(this->get_mutable_attrs().owner);
   if(this->is_spell()) {
      total_mana += state->get_spell_mana(this->get_mutable_attrs().owner);
   }
   return this->get_mana_cost() <= total_mana && _check_play_condition(game);
}
std::vector< sptr< Grant > > Card::get_all_grants() const
{
   return get_mutable_attrs().grants_temp + get_mutable_attrs().grants;
}
void Card::store_grant(const sptr< Grant >& grant)
{
   if(grant->is_permanent()) {
      m_mutables.grants.emplace_back(grant);
   } else {
      m_mutables.grants_temp.emplace_back(grant);
   }
}
bool Card::operator()(Game& game, const events::AnyEvent& event)
{
   bool all_consumed = true;
   for(auto& effect : m_mutables.effects.at(event.get_event_type())) {
      if(not effect.is_consumed()) {
         effect(game, event);
         all_consumed = false;
      }
   }
   return all_consumed;
}
