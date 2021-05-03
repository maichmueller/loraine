#ifndef LORAINE_UTIlS_H
#define LORAINE_UTIlS_H

#include <optional>
#include <tuple>
#include <variant>
#include <set>

#include "random.h"
#include "algorithms.h"
#include "types.h"


template < typename Key, typename Compare, typename Allocator >
std::set< Key, Compare, Allocator > operator+(
   std::set< Key, Compare, Allocator >& set1,
   const std::set< Key, Compare, Allocator >& set2)
{
   std::merge(set1.begin(), set1.end(),set2.begin(), set2.end(), std::back_inserter(set1));
   return set1;
}

template < typename Key, typename Compare, typename Allocator >
std::set< Key, Compare, Allocator > operator+(
   const std::set< Key, Compare, Allocator >& set1,
   const std::set< Key, Compare, Allocator >& set2)
{
   decltype(set1) out = set1;
   return out + set2;
}
template < typename Key, typename Compare, typename Allocator >
std::set< Key, Compare, Allocator > operator+(
   std::set< Key, Compare, Allocator >&& set1,
   std::set< Key, Compare, Allocator >&& set2)
{
   return set1 + set2;
}





template < typename T, typename Allocator >
std::vector< T, Allocator >& operator+(
   std::vector< T, Allocator >& vec1,
   const std::vector< T, Allocator >& vec2)
{
   vec1.reserve(vec1.size() + vec2.size());
   for(const auto& elem : vec2) {
      vec1.emplace_back(elem);
   }
   return vec1;
}
template < typename T, typename Allocator >
std::vector< T, Allocator > operator+(
   const std::vector< T, Allocator >& vec1,
   const std::vector< T, Allocator >& vec2)
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
   std::vector< T, Allocator >& vec1,
   std::vector< T, Allocator >&& vec2)
{
   vec1.reserve(vec1.size() + vec2.size());
   for(auto&& elem : vec2) {
      vec1.emplace_back(std::move(elem));
   }
   return vec1;
}
template < typename T, typename Allocator >
std::vector< T, Allocator >& operator+(
   std::vector< T, Allocator >&& vec1,
   std::vector< T, Allocator >&& vec2)
{
   vec1.reserve(vec1.size() + vec2.size());
   for(auto&& elem : vec2) {
      vec1.emplace_back(std::move(elem));
   }
   return vec1;
}

namespace utils {


template < template < typename... > class, typename >
struct pass_args;

template < template < typename... > class OutClass, typename... T >
struct pass_args< OutClass, std::tuple< T... > > {
   using type = OutClass< T... >;
};

template < typename... Args >
struct deduction_help;

template < class T, class... Ts >
struct is_any: std::disjunction< std::is_same< T, Ts >... > {
};
template < class T, class... Ts >
inline constexpr bool is_any_v = is_any< T, Ts... >::value;

template < class T, class... Ts >
struct are_same: std::conjunction< std::is_same< T, Ts >... > {
};
template < class T, class... Ts >
inline constexpr bool are_same_v = are_same< T, Ts... >::value;

template < std::size_t I, class T >
struct variant_element;

// recursive case
template < std::size_t I, class Head, class... Tail >
struct variant_element< I, std::variant< Head, Tail... > >:
    variant_element< I - 1, std::variant< Tail... > > {
};

// base case
template < class Head, class... Tail >
struct variant_element< 0, std::variant< Head, Tail... > > {
   using type = Head;
};

template < std::size_t I, class T >
using variant_element_t = typename variant_element< I, T >::type;

template < template < typename... > class Base, typename Derived, typename... Args >
struct CRTP {
   Derived* derived() { return static_cast< Derived* >(this); }
   Derived const* derived() const { return static_cast< Derived const* >(this); }

  protected:
   constexpr CRTP() = default;
   friend Base< Derived, Args... >;
};

template < size_t N, typename T >
struct getter {
   auto operator()(const T& t) { return std::get< N >(t); }
   using type = std::invoke_result_t< getter(const T&) >;
};
template < size_t N, typename T >
using getter_t = getter< N, T >;

template < typename IntType, typename = std::enable_if_t< std::is_integral_v< IntType > > >
inline IntType clamp(IntType value, IntType min, IntType max)
{
   return std::max(min, std::min(max, value));
}

template < typename T >
inline bool has_value(const sptr< T >& ptr)
{
   return ptr != nullptr;
}

template < typename T >
inline bool has_value(const uptr< T >& ptr)
{
   return ptr != nullptr;
}
template < typename T >
inline bool has_value(const std::optional< T >& t)
{
   return t.has_value();
}
template < typename T >
inline void throw_if_no_value(
   const sptr< T >& ptr,
   const std::string& msg = "Shared pointer is nullptr.")
{
   if(not has_value(ptr)) {
      throw std::logic_error(msg);
   }
}
template < typename T >
inline void throw_if_no_value(
   const uptr< T >& ptr,
   const std::string& msg = "Unique pointer is nullptr.")
{
   if(not has_value(ptr)) {
      throw std::logic_error(msg);
   }
}
template < typename T >
inline void throw_if_no_value(const std::optional< T >& ptr)
{
   if(not has_value(ptr)) {
      throw std::logic_error("Passed optional holds no value.");
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
 * iterator. Because an empty range is provided, the call to erase does not change the contents
 * of the ContainerType.
 */
template < typename Container, typename ConstIterator >
typename Container::iterator remove_constness(Container& c, ConstIterator it)
{
   return c.erase(it, it);
}

}  // namespace utils

#endif  // LORAINE_UTILS_H