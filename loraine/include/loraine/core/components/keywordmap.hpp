
#ifndef LORAINE_KEYWORDMAP_HPP
#define LORAINE_KEYWORDMAP_HPP

#include "loraine/utils/utils.h"

class KeywordMap {
  public:
   KeywordMap() : m_kw_arr() {}  // all keyword positions marked false by default
   constexpr KeywordMap(std::array< bool, n_keywords > keywords) : m_kw_arr(keywords) {}
   template < typename Container >
   KeywordMap(const Container& keywords) : m_kw_arr()
   {
      algo::for_each([&](Keyword kw) { add(kw); }, keywords);
   }

   [[nodiscard]] inline bool has(Keyword kword) const { return m_kw_arr.at(_to_int(kword)); }

   template < typename Container, typename = std::enable_if_t< Container::value_type, Keyword > >
   [[nodiscard]] inline bool has_any_of(Container kwords) const
   {
      return algo::any_of(kwords, [&](const auto& kw) { return has(kw); });
   }

   inline void add(Keyword kw) { m_kw_arr[_to_int(kw)] = true; }
   template < typename Container, typename = std::enable_if_t< Container::value_type, Keyword > >
   inline void add(Container kwords)
   {
      algo::transform(&KeywordMap::add, kwords);
   }
   inline void remove(Keyword kw) { m_kw_arr[_to_int(kw)] = false; }
   template < typename Container, typename = std::enable_if_t< Container::value_type, Keyword > >
   inline void remove(Container kwords)
   {
      algo::transform(&KeywordMap::remove, kwords);
   }

  private:

   /// members

   /// a bit-map of n_keywords length
   std::array< bool, n_keywords > m_kw_arr;

   /// private methods

   inline size_t _to_int(Keyword kw) const { return static_cast< size_t >(kw); }
};


#endif  // LORAINE_KEYWORDMAP_HPP
