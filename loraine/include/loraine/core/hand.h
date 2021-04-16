#ifndef LORAINE_HAND_H
#define LORAINE_HAND_H

#include <vector>
#include <entt/entt.hpp>

struct Hand : private std::vector< entt::entity> {
   using base = std::vector< entt::entity >;

   /// constructors

   Hand(size_t max_size = std::numeric_limits< size_t >::max()) : m_max_size(max_size) {}
   Hand(const Hand& other) = default;
   Hand& operator=(const Hand& other) = delete;
   Hand(Hand&& other) = default;
   Hand& operator=(Hand&& other) = default;
   ~Hand() = default;

   /// forwarded functions from container

   using base::size;
   using base::begin;
   using base::end;
   using base::operator[];
   using base::at;
   using base::erase;
   using base::empty;
   using base::front;
   using base::back;

   // own added methods

   inline bool add(entt::entity card)
   {
      if(size() == m_max_size) {
         return false;
      }
      emplace_back(card);
      return true;
   }

  private:
   size_t m_max_size;
};

#endif  // LORAINE_HAND_H
