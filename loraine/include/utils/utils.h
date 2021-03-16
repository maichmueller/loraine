
#ifndef LORAINE_UTIlS_H
#define LORAINE_UTIlS_H

#include <uuid.h>

#include <optional>
#include <tuple>
#include <variant>

#include "random.h"
#include "types.h"

namespace utils {
inline UUID new_uuid()
{
   // safer to wrap static variable in function call, in case of exception throw.
   static auto generator = uuids::uuid_random_generator();
   return generator();
}

template < template < typename... > class, typename >
struct pass_args;

template < template < typename... > class OutClass, typename... T >
struct pass_args< OutClass, std::tuple< T... > > {
   using type = OutClass< T... >;
};

template < typename... Args >
struct deduction_help {
  private:
   std::tuple< Args... > t;
};

template < template < typename... > class Base, typename Derived, typename... Args >
struct CRTP {
   Derived* derived() { return static_cast< Derived* >(*this); }
   Derived const* derived() const { return static_cast< Derived const* >(*this); }

  private:
   constexpr CRTP() = default;
   friend Base< Derived, Args... >;
};

template < size_t N, typename T >
struct getter {
   auto operator()(const T& t) { return std::get< N >(t); }
   using type = std::result_of_t< getter(const T&) >;
};
template < size_t N, typename T >
using getter_t = getter< N, T >;

// template < typename, typename = void >
// struct has_sort_method: std::false_type {
//};
//
// template < typename T >
// struct has_sort_method< T, std::void_t< decltype(&T::sort_subscribers) > >:
//   std::is_same< _sort_func_return_type, decltype(std::declval< T >().sort_subscribers()) > {
//};

template < typename T >
inline bool has_value(const sptr< T >& ptr)
{
   return ptr != nullptr;
}
template < typename T >
inline bool has_value(const std::optional< T >& t)
{
   return t.has_value();
}
template < typename T >
inline void throw_if_no_value(const sptr< T >& ptr)
{
   if(not has_value(ptr)) {
      throw std::logic_error("Shared pointer is null.");
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


template< std::size_t I, class T >
struct variant_element;

// recursive case
template< std::size_t I, class Head, class... Tail >
struct variant_element<I, std::variant<Head, Tail...>>
   : variant_element<I-1, std::variant<Tail...>> { };

// base case
template< class Head, class... Tail >
struct variant_element<0, std::variant<Head, Tail...>> {
   using type = Head;
};

template< std::size_t I, class T >
using variant_element_t = typename variant_element<I, T>::type;


}  // namespace utils

#endif  // LORAINE_UTILS_H