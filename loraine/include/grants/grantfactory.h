

#ifndef LORAINE_GRANTFACTORY_H
#define LORAINE_GRANTFACTORY_H

#include "grant.h"
#include "grantmodifier.h"

class GrantFactory {
  public:
   template < GrantType grant_type, typename... Params >
   inline sptr< Grant > grant(
      const sptr< Card >& bestowing_card, const sptr< Card >& card_to_bestow, Params&&... params)
   {
      sptr< Grant > grant = nullptr;
      if constexpr(grant_type == GrantType::STATS) {
         grant = std::make_shared< StatsGrant >(
            grant_type, bestowing_card, card_to_bestow, std::forward< Params... >(params...));
      } else if constexpr(grant_type == GrantType::MANA) {
         grant = std::make_shared< ManaGrant >(
            grant_type, bestowing_card, card_to_bestow, std::forward< Params... >(params...));
      } else if constexpr(grant_type == GrantType::KEYWORD) {
         grant = std::make_shared< KeywordGrant >(
            grant_type, bestowing_card, card_to_bestow, std::forward< Params... >(params...));
      } else if constexpr(grant_type == GrantType::EFFECT) {
         grant = std::make_shared< EffectGrant >(
            grant_type, bestowing_card, card_to_bestow, std::forward< Params... >(params...));
      }
      for(auto& modifier : m_modifiers) {
         (*modifier)(*grant);
      }
      grant->apply();
      card_to_bestow->store_grant(grant);
      return grant;
   }

   inline void add_modifier(sptr< GrantModifier > modifier)
   {
      m_modifiers.emplace_back(std::move(modifier));
   }
   inline void remove_modifier(sptr< GrantModifier > modifier)
   {
      if(auto pos = std::find_if(
            m_modifiers.begin(),
            m_modifiers.end(),
            [&](const sptr< GrantModifier >& mod) { return mod == modifier; });
         pos != m_modifiers.end()) {
         m_modifiers.erase(pos);
      }
   }

  private:
   std::vector< sptr< GrantModifier > > m_modifiers;
};

#endif  // LORAINE_GRANTFACTORY_H
