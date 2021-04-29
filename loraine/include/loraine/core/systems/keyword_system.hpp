

#ifndef LORAINE_KEYWORD_SYSTEM_HPP
#define LORAINE_KEYWORD_SYSTEM_HPP

#include "loraine/core/gamedefs.h"
#include "loraine/events/event_id.h"
#include "system.hpp"

class KeywordSystem: public ILogicSystem {
   KeywordSystem(entt::registry& registry) : ILogicSystem(registry) {}


   template < Keyword keyword >
   void on([[maybe_unused]] entt::registry& registry, entt::entity card)
   {
      _simply_add< keyword >(card);
   }

   template <>
   void on< Keyword::DEEP >([[maybe_unused]] entt::registry& registry, entt::entity card)
   {
      _simply_add< Keyword::DEEP >(card);
      m_registry.emplace< tag::subscriber< events::EventID::DEEP > >(card);
   }

   template < Keyword keyword >
   void off([[maybe_unused]] entt::registry& registry, entt::entity card)
   {
      _simply_remove< keyword >(card);
   }

  private:
   template < Keyword keyword >
   inline void _simply_add(entt::entity card)
   {
      m_registry.get< KeywordMap >(card).add(keyword);
   }
   template < Keyword keyword >
   inline void _simply_remove(entt::entity card)
   {
      m_registry.get< KeywordMap >(card).remove(keyword);
   }
};

#endif  // LORAINE_KEYWORD_SYSTEM_HPP
