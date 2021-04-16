#ifndef LORAINE_HAND_H
#define LORAINE_HAND_H

struct Hand {
   using ContainerType = std::vector< entt::entity >;

   using value_type = typename ContainerType::value_type;
   using pointer = typename ContainerType::pointer;
   using reference = typename ContainerType::reference;
   using const_reference = typename ContainerType::const_reference;
   using iterator = typename ContainerType::iterator;
   using const_iterator = typename ContainerType::const_iterator;
   using reverse_iterator = typename ContainerType::reverse_iterator;
   using const_reverse_iterator = typename ContainerType::const_reverse_iterator;
   using size_type = typename ContainerType::size_type;
   using difference_type = typename ContainerType::difference_type;

   /// constructors
   Hand(size_t max_size = std::numeric_limits< size_t >::max()) : m_max_size(max_size) {}

   Hand(const Hand& other) = default;
   Hand& operator=(const Hand& other) = delete;
   Hand(Hand&& other) = default;
   Hand& operator=(Hand&& other) = default;
   ~Hand() = default;

   /// forwarded functions from container

   [[nodiscard]] inline auto size() const noexcept { return m_cards.size(); }
   [[nodiscard]] inline auto begin() const noexcept { return m_cards.begin(); }
   [[nodiscard]] inline auto begin() noexcept { return m_cards.begin(); }
   [[nodiscard]] inline auto end() const noexcept { return m_cards.end(); }
   [[nodiscard]] inline auto end() noexcept { return m_cards.end(); }
   [[nodiscard]] inline auto& operator[](size_t n) { return m_cards[n]; }
   [[nodiscard]] inline auto& operator[](size_t n) const { return m_cards[n]; }
   [[nodiscard]] inline const auto& at(size_t idx) const { return m_cards.at(idx); }
   [[nodiscard]] inline auto& at(size_t idx) { return m_cards.at(idx); }
   [[nodiscard]] inline auto erase(const_iterator position) { return m_cards.erase(position); }
   [[nodiscard]] inline auto empty() const noexcept { return m_cards.empty(); }

   inline bool add(entt::entity card)
   {
      if(size() == m_max_size) {
         return false;
      }
      m_cards.emplace_back(card);
      return true;
   }

  private:
   std::vector< entt::entity > m_cards;
   size_t m_max_size;
};

#endif  // LORAINE_HAND_H
