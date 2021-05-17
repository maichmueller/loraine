

#ifndef LORAINE_KEYWORD_SYSTEM_HPP
#define LORAINE_KEYWORD_SYSTEM_HPP

#include "loraine/core/gamedefs.h"
#include "loraine/events/event_id.h"
#include "system.hpp"

class KeywordSystem: public ILogicSystem {
   KeywordSystem() = default;


   template < Keyword keyword >
   void on_add([[maybe_unused]] entt::registry& registry, entt::entity card)
   {
      _simply_add< keyword >(card);
   }

   template < Keyword keyword >
   void on_remove([[maybe_unused]] entt::registry& registry, entt::entity card)
   {
      _simply_remove< keyword >(card);
   }

  private:
   template < Keyword keyword >
   inline void _simply_add(entt::entity card)
   {
      m_registry->get< KeywordMap >(card).add(keyword);
   }
   template < Keyword keyword >
   inline void _simply_remove(entt::entity card)
   {
      m_registry->get< KeywordMap >(card).remove(keyword);
   }
};


template <>
void KeywordSystem::on_add< Keyword::DEEP >([[maybe_unused]] entt::registry& registry, entt::entity card)
{
   _simply_add< Keyword::DEEP >(card);
   m_registry->emplace< tag::subscriber< events::EventID::DEEP > >(card);
}

#endif  // LORAINE_KEYWORD_SYSTEM_HPP
