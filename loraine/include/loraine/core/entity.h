
#ifndef LORAINE_ENTITY_H
#define LORAINE_ENTITY_H

#include <entt/entt.hpp>

class Entity {
   Entity(entt::registry& registry) : m_id(registry.create()) {
   }

   template <typename Component>
   bool has(entt::registry& registry) {
      return registry.template try_get<Component>(m_id) == nullptr;
   }
   template <typename Component>
   auto& get(entt::registry& registry) {
      return registry.template get<Component>(m_id);
   }

   template <typename Component, typename...Args>
   inline auto& add(entt::registry& registry, Args&&...args){
      return registry.template emplace<Component>(std::forward<Args>(args)...);
   }

   template <typename Component>
   inline void remove(entt::registry& registry) {
      registry.remove<Component>(m_id);
   }

   inline operator entt::entity() { return m_id;}

  private:
   entt::entity m_id;
};

#endif  // LORAINE_ENTITY_H
