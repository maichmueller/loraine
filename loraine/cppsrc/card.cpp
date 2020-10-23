
#include "cards/card.h"

#include <utility>

#include "cards/keywords.h"
#include "game.h"
#include "utils.h"

Card::Card(
   Player owner,
   const char* const code,
   const char* const name,
   const char* const effect_desc,
   const char* const lore,
   Region region,
   Group group,
   CardSuperType super_type,
   Rarity rarity,
   CardType card_type,
   bool is_collectible,
   size_t mana_cost,
   std::initializer_list< enum Keyword > keyword_list,
   std::map< events::EventType, std::vector< EffectContainer > >  effects,
   bool is_hidden)
    : m_name(name),
      m_effect_desc(effect_desc),
      m_lore(lore),
      m_region(region),
      m_group(group),
      m_super_type(super_type),
      m_rarity(rarity),
      m_card_type(card_type),
      m_is_collectible(is_collectible),
      m_hidden(is_hidden),
      m_code(code),
      m_uuid(new_uuid()),
      m_mana_cost_ref(mana_cost),
      m_mana_cost_base(mana_cost),
      m_mana_cost_delta(0),
      m_keywords(create_kword_list(keyword_list)),
      m_effects(std::move(effects)),
      m_owner(owner)
{
}
Card::Card(const Card& card)
    : m_name(card.get_name()),
      m_effect_desc(card.get_effect_desc()),
      m_lore(card.get_lore()),
      m_region(card.get_region()),
      m_group(card.get_group()),
      m_super_type(card.get_super_type()),
      m_rarity(card.get_rarity()),
      m_card_type(card.get_card_type()),
      m_is_collectible(card.is_collectible()),
      m_code(card.get_id()),
      m_hidden(card.is_hidden()),
      m_uuid(new_uuid()),
      m_mana_cost_ref(card.get_mana_cost_ref()),
      m_mana_cost_base(card.get_mana_cost_base()),
      m_mana_cost_delta(card.get_mana_cost_delta()),
      m_keywords(card.get_keywords()),
      m_effects(card.get_effects_map()),
      m_owner(card.get_owner())
{
}
void Card::remove_effect(events::EventType e_type, const EffectContainer& effect)
{
   if(auto eff_vec_iter = std::find_if(
         m_effects.begin(), m_effects.end(), [&](const auto& val) { return val.first == e_type; });
      eff_vec_iter != m_effects.end()) {
      auto& eff_vec = (*eff_vec_iter).second;
      auto position = std::find(eff_vec.begin(), eff_vec.end(), effect);
      if(position != eff_vec.end()) {
         if(eff_vec.size() == 1) {
            m_effects.erase(eff_vec_iter);
         } else {
            eff_vec.erase(position);
         }
      }
   }
}
void Card::add_effect(events::EventType e_type, EffectContainer effect)
{
   // if the key is already found in the effects map, delete the previous
   // effect. This essentially implies we overwrite preexisting effects
   if(m_effects.find(e_type) != m_effects.end()) {
      m_effects.erase(e_type);
   }
   m_effects[e_type].emplace_back(std::move(effect));
}
void Card::set_effect_vec(events::EventType e_type, std::vector< EffectContainer > effects)
{
   auto& curr_vec = m_effects[e_type];
   for(auto&& eff : effects) {
      curr_vec.emplace_back(std::move(eff));
   }
}
bool Card::check_play_condition(const Game& game) const
{
   auto state = game.get_state();
   size_t total_mana = state->get_mana(this->get_owner());
   if(this->is_spell()) {
      total_mana += state->get_spell_mana(this->get_owner());
   }
   return this->get_mana_cost() <= total_mana && _check_play_condition(game);
}

bool Unit::_check_play_condition(const Game& game) const
{
   return game.get_state()->get_mana(get_owner()) >= get_mana_cost();
}
void Unit::add_power(long amount, bool permanent)
{
   if(permanent) {
      m_power_base += amount;
   } else {
      m_power_delta += amount;
   }
}
void Unit::add_health(long amount, bool permanent)
{
   if(permanent) {
      m_health_base += amount;

   } else {
      m_health_delta += amount;
   }
}
void Unit::set_health(size_t health)
{
   m_health_delta = static_cast< decltype(m_health_delta) >(health - m_health_base);
}
void Unit::set_power(size_t power, bool as_delta)
{
   if(as_delta) {
      m_power_delta = static_cast< decltype(m_power_delta) >(power - m_power_base);
   } else {
      m_power_base = power;
      m_power_delta = 0;
   }
}
Unit::Unit(
   Player owner,
   const char* const code,
   const char* const name,
   const char* const effect_desc,
   const char* const lore,
   Region region,
   Group group,
   CardSuperType super_type,
   Rarity rarity,
   bool is_collectible,
   size_t mana_cost_ref,
   size_t power_ref,
   size_t health_ref,
   const std::initializer_list< enum Keyword >& keyword_list,
   const std::map< events::EventType, std::vector< EffectContainer > >& effects,
   CardType card_type)
    : Card(
       owner,
       code,
       name,
       effect_desc,
       lore,
       region,
       group,
       super_type,
       rarity,
       card_type,
       is_collectible,
       mana_cost_ref,
       keyword_list,
       effects),
      m_power_ref(power_ref),
      m_power_base(power_ref),
      m_health_ref(health_ref),
      m_health_base(health_ref)
{
}
Unit::Unit(const Unit& card)
    : Card(card),
      m_power_ref(card.get_power_ref()),
      m_power_base(card.get_power_base()),
      m_health_ref(card.get_health_ref()),
      m_health_base(card.get_health_base()),
      m_power_delta(card.get_power_delta()),
      m_health_delta(card.get_health_delta()),
      m_damage(card.get_damage())
{
}
Spell::Spell(
   Player owner,
   const char* const code,
   const char* const name,
   const char* const effect_desc,
   const char* const lore,
   Region region,
   Group group,
   CardSuperType super_type,
   Rarity rarity,
   bool is_collectible,
   size_t mana_cost,
   const std::initializer_list< enum Keyword >& keyword_list,
   const std::map< events::EventType, std::vector< EffectContainer > >& effects)
    : Card(
       owner,
       code,
       name,
       effect_desc,
       lore,
       region,
       group,
       super_type,
       rarity,
       CardType::SPELL,
       is_collectible,
       mana_cost,
       keyword_list,
       std::move(effects))
{
}
bool Spell::_check_play_condition(const Game& game) const
{
   // If any of the sub-effects of the spell are playable then this is the generally accepted case.
   // Any specific spell can choose to override this in its inheritance.
   for(const auto& event_effect : get_effects_map()) {
      const auto& effect_vec = event_effect.second;
      for(const auto& effect : effect_vec) {
         if(effect.check_cast_condition(game, events::NoneEvent())) {
            return true;
         }
      }
   }
   return false;
}

Landmark::Landmark(
   Player owner,
   const char* const code,
   const char* const name,
   const char* const effect_desc,
   const char* const lore,
   Region region,
   Group group,
   Rarity rarity,
   bool is_collectible,
   size_t mana_cost,
   std::initializer_list< enum Keyword > keyword_list,
   const std::map< events::EventType, std::vector< EffectContainer > >& effects)
    : Unit(
       owner,
       code,
       name,
       effect_desc,
       lore,
       region,
       group,
       CardSuperType::NONE,
       rarity,
       is_collectible,
       mana_cost,
       -1,
       -1,
       keyword_list,
       effects,
       CardType::LANDMARK)
{
   add_keyword(Keyword::LANDMARK);
}
