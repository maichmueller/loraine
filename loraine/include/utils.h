
#ifndef LORAINE_UTIlS_H
#define LORAINE_UTIlS_H

#include <optional>

#include "rng_machine.h"
#include "types.h"

template < typename T >
inline bool has_value(const sptr< T >& ptr)
{
   return ptr != nullptr;
}
template < typename T >
inline bool has_value(const std::optional< T >& ptr)
{
   return ptr.has_value();
}
template < typename T >
inline void throw_if_no_value(const sptr< T >& ptr)
{
   if(not has_value(ptr)) {
      throw std::logic_error("Shared pointer is null.");
   }
}

template < typename T >
inline void reset(sptr< T >& ptr)
{
   ptr = nullptr;
}

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
   for(const auto& elem : vec2) {
      vec1.emplace_back(elem);
   }
   return vec1;
}
template < typename T, typename Allocator >
std::vector< T, Allocator > operator+(
   const std::vector< T, Allocator >& vec1, const std::vector< T, Allocator >& vec2)
{
   auto vec = vec1;
   vec.reserve(vec1.size() + vec2.size());
   for(const auto& elem : vec2) {
      vec.emplace_back(elem);
   }
   return vec;
}
template < typename T, typename Allocator >
std::vector< T, Allocator >& operator+(
   std::vector< T, Allocator >& vec1, std::vector< T, Allocator >&& vec2)
{
   vec1.reserve(vec1.size() + vec2.size());
   for(auto&& elem : vec2) {
      vec1.emplace_back(std::move(elem));
   }
   return vec1;
}
template < typename T, typename Allocator >
std::vector< T, Allocator >& operator+(
   std::vector< T, Allocator >&& vec1, std::vector< T, Allocator >&& vec2)
{
   vec1.reserve(vec1.size() + vec2.size());
   for(auto&& elem : vec2) {
      vec1.emplace_back(std::move(elem));
   }
   return vec1;
}

template < typename Container >
void shuffle_inplace(Container& container)
{
   std::shuffle(container.begin(), container.end(), rng::get_engine());
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

/*
 * Function to remove the const qualifier of a provided iterator
 *
 * The range-erase member functions have a pair of const_iterator parameters, but they return an
 * iterator. Because an empty range is provided, the call to erase does not change the contents of
 * the container.
 */
template < typename Container, typename ConstIterator >
typename Container::iterator remove_constness(Container& c, ConstIterator it)
{
   return c.erase(it, it);
}

inline bool bernoulli_sample(double p)
{
   std::bernoulli_distribution ber(p);
   return ber(rng::get_engine());
}

template < class BiIter, typename Distribution = std::uniform_int_distribution< uint64_t > >
void shuffle_inplace_limited(
   BiIter begin, BiIter end, size_t num_random, Distribution weight_dist = Distribution(0))
{
   // Fisher-Yates-shuffle
   size_t N = std::distance(begin, end);
   while(num_random--) {
      BiIter r = begin;
      std::advance(r, weight_dist(rng::get_engine()) % N);
      std::swap(*begin, *r);
      ++begin;
      --N;
   }
}

template < typename T, typename Allocator >
void choose_inplace(std::vector< T, Allocator >& vec, int n = 1)
{
   shuffle_inplace_limited(vec.begin(), vec.end(), n);
}
template < typename T, typename Allocator >
void choose_inplace(std::vector< T, Allocator >& vec, std::vector< double > weights, int n = 1)
{
   shuffle_inplace_limited(
      vec.begin(),
      vec.end(),
      n,
      std::discrete_distribution< uint64_t >{weights.begin(), weights.end()});
}
template < typename T, typename Allocator >
T choose(const std::vector< T, Allocator >& vec, int n = 1)
{
   std::vector< size_t > indices(vec.size());
   std::iota(indices.begin(), indices.end(), 0);
   shuffle_inplace_limited(indices.begin(), indices.end(), n);
   std::vector< T, Allocator > out;
   out.reserve(n);
   for(auto idx : indices) {
      out.emplace_back(vec[idx]);
   }
   return out;
}

#endif  // LORAINE_UTILS_H