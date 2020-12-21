

#ifndef LORAINE_FACTORY_H
#define LORAINE_FACTORY_H

#include "cards/grant.h"

class GrantFactory {
  public:
   template < GrantType grant_type, typename... Params >
   inline sptr< Grant > grant(
      const sptr< Card >& bestowing_card, const sptr< Card >& card_to_bestow, Params&&... params)
   {
      sptr< Grant > grant = nullptr;
      if constexpr(grant_type == STATS) {
         grant = std::make_shared< StatsGrant >(
            grant_type, bestowing_card, card_to_bestow, std::forward< Params... >(params...));
      } else if constexpr(grant_type == MANA) {
         grant = std::make_shared< ManaGrant >(
            grant_type, bestowing_card, card_to_bestow, std::forward< Params... >(params...));
      } else if constexpr(grant_type == KEYWORD) {
         grant = std::make_shared< KeywordGrant >(
            grant_type, bestowing_card, card_to_bestow, std::forward< Params... >(params...));
      } else if constexpr(grant_type == EFFECT) {
         grant = std::make_shared< EffectGrant >(
            grant_type, bestowing_card, card_to_bestow, std::forward< Params... >(params...));
      }
      for(auto&& adj : m_adjustments) {
         adj(grant, card_to_bestow, bestowing_card);
      }
      card_to_bestow->store_grant(grant);
      return grant;
   }

  private:
   std::vector< std::function< void(sptr< Grant >&, const sptr< Card >&, const sptr< Card >&) > >
   m_adjustments;
};



#endif  // LORAINE_FACTORY_H
