
template < typename T >
struct reversion_wrapper {
   T& iterable;
   auto begin() { return std::rbegin(iterable); }
   auto end() { return std::rend(iterable); }
};
template < typename T >
reversion_wrapper< T > reverse(T&& iterable)
{
   return {iterable};
}

template < typename T, typename Allocator >
std::vector< T, Allocator >& operator+(
   std::vector< T, Allocator >& vec1, const std::vector< T, Allocator >& vec2)
{
   vec1.reserve(vec1.size() + vec2.size());
   for(const auto & elem : vec2) {
      vec1.emplace_back(elem);
   }
   return vec1;
}
template < typename T, typename Allocator >
std::vector< T, Allocator >& operator+(
   std::vector< T, Allocator >& vec1, std::vector< T, Allocator >&& vec2)
{
   vec1.reserve(vec1.size() + vec2.size());
   for(auto && elem : vec2) {
      vec1.emplace_back(std::move(elem));
   }
   return vec1;
}