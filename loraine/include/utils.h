
#ifndef LORAINE_UTIlS_H
#define LORAINE_UTIlS_H

#include "rng_machine.h"

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

template < typename Container >
void shuffle_inplace(Container& container)
{
   std::shuffle(container.begin(), container.end(), rng::rng_def_engine);
}

/*
 * Get the address of the function pointer.
 *
 * Needed for e.g. effect function comparison of equality.
 *
 * this may need some further checkups for portability, as conversion to
 * size_t of a function pointer is implementation dependent.
 */
template < typename ReturnType, typename... Params >
size_t get_address(std::function< ReturnType(Params...) > f)
{
   using FuncType = ReturnType(Params...);
   auto** fn_ptr = f.template target< FuncType* >();
   return size_t(*fn_ptr);
}


#endif  // LORAINE_UTILS_H