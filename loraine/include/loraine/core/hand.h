#ifndef LORAINE_HAND_H
#define LORAINE_HAND_H

#include <entt/entt.hpp>
#include <vector>

struct Hand final: private std::vector< entt::entity > {

   using base = std::vector< entt::entity >;

  public:

   // forwarding traits;

   using base::value_type;
   using base::pointer;
   using base::const_pointer;
   using base::reference;
   using base::const_reference;
   using base::iterator;
   using base::const_iterator;
   using base::const_reverse_iterator;
   using base::reverse_iterator;
   using base::size_type;
   using base::difference_type;
   using base::allocator_type;

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
