

#ifndef LORAINE_ALGORITHMS_H
#define LORAINE_ALGORITHMS_H

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace algo {

template < typename Container, typename FilterFunc >
auto find_indices(const Container& container, const FilterFunc& filter) -> std::vector< long >
{
   std::vector< long > indices;
   for(long i = 0; i < container.size(); ++i) {
      if(filter(container[i])) {
         indices.push_back(i);
      }
   }
   return indices;
}
template < typename Container, typename T >
auto find_indices(const Container& container, const std::vector< T >& elems) -> std::vector< long >
{
   auto nr_elems = elems.size();
   std::vector< long > indices(nr_elems, -1);
   std::vector< bool > found(nr_elems, false);
   for(long i = 0; i < container.size(); ++i) {
      for(long elem_idx = 0; elem_idx < nr_elems; ++elem_idx) {
         if(not found[elem_idx]) {
            if(container[i] == elems[elem_idx]) {
               indices.push_back(i);
               found[elem_idx] = true;
            }
         }
      }
   }
   return indices;
}

//// constexpr versions of the std functions (would need c++20 to be constexpr)
//template < typename InputIter, typename T >
//constexpr auto find(InputIter begin, InputIter end, const T& elem) noexcept
//{
//   for(; begin != end; ++begin) {
//      if constexpr(*begin == elem)
//         return begin;
//   }
//   return end;
//}
//
//template < typename InputIter, typename Predicate >
//constexpr auto find_if(InputIter begin, InputIter end, Predicate pred) noexcept
//{
//   for(; begin != end; ++begin) {
//      if(pred(*begin))
//         return begin;
//   }
//   return end;
//}
//
template < typename Container, typename T >
inline constexpr auto find(const Container& container, const T& elem) noexcept
{
   return std::find(container.begin(), container.end(), elem);
}

template < typename Container, typename Predicate >
inline constexpr auto find_if(const Container& container, Predicate pred) noexcept
{
   return std::find_if(container.begin(), container.end(), pred);
}

template < typename Container, typename T >
inline auto find_index(const Container& container, const T& elem) -> long
{
   auto pos = find(container, elem);
   return pos == container.end() ? -1 : std::distance(container.begin(), pos);
}

template < template < typename, typename > class Container, typename T1, typename T2 >
void permute(Container< T1, T2 >& vec_like, std::vector< int >& perm_order)
{
   for(size_t i = 0; i < perm_order.size(); i++) {
      auto current = i;
      while(i != perm_order[current]) {
         auto next = perm_order[current];
         std::swap(vec_like[current], vec_like[next]);
         perm_order[current] = current;
         current = next;
      }
      perm_order[current] = current;
   }
}

template < typename Container, typename T, typename FilterFunc >
auto pop_if(Container& container, const FilterFunc& filter) -> std::vector< T >
{
   std::vector< T > popped_elems;
   auto partition_begin = std::partition(container.begin(), container.end(), std::not_fn(filter));
   std::copy(partition_begin, container.end(), std::back_inserter(popped_elems));
   container.erase(partition_begin, container.end());
   shuffle_inplace(container);
   return popped_elems;
}

template < typename Container, typename T, typename FilterFunc >
inline auto copy_if(const Container& container, const FilterFunc& filter) -> std::vector< T >
{
   std::vector< T > elements;
   std::copy_if(container.begin(), container.end(), std::back_inserter(elements), filter);
   shuffle_inplace(container);
   return elements;
}

template < typename Container, typename T >
inline void remove_element(Container& container, const T& elem)
{
   auto pos = algo::find(container, elem);
   if(pos == container.end()) {
      throw std::logic_error("Element to remove was not found in ContainerType.");
   }
   container.erase(pos);
}

template < class InputIt, class UnaryPredicate >
inline constexpr bool any_of(InputIt first, InputIt last, UnaryPredicate p)
{
   return find_if(first, last, p) != last;
}

template < class Container, class UnaryPredicate >
inline constexpr bool any_of(const Container& container, UnaryPredicate p)
{
   return find_if(container.begin(), container.end(), p) != container.end();
}

template < class Container >
inline constexpr bool contains(
   const Container& container,
   const typename Container::value_type& value)
{
   return std::find(container.begin(), container.end(), value) != container.end();
}

template < class InputIt, class UnaryPredicate >
inline constexpr bool all_of(InputIt first, InputIt last, UnaryPredicate p)
{
   return std::find_if_not(first, last, p) == last;
}

template < class Container, class UnaryPredicate >
inline constexpr bool all_of(const Container& container, UnaryPredicate p)
{
   return std::find_if_not(container.begin(), container.end(), p) == container.end();
}

template < class InputIt, class UnaryPredicate >
inline constexpr bool none_of(InputIt first, InputIt last, UnaryPredicate p)
{
   return not any_of(first, last, p);
}

template < class Container, class UnaryPredicate >
inline constexpr bool none_of(const Container& container, UnaryPredicate p)
{
   return not any_of(container, p);
}

template < typename Predicate, typename Container >
inline Container transform(Predicate f, Container& container)
{
   std::transform(container.begin(), container.end(), container.begin(), f);
   return container;
}
template < typename Predicate, typename Container, typename OutIter >
inline Container transform(Predicate f, const Container& container, OutIter out_iter)
{
   std::transform(container.begin(), container.end(), out_iter, f);
   return container;
}

template < typename Predicate, typename Container >
inline std::vector< std::invoke_result_t< Predicate > > apply(
   Predicate f,
   const Container& container)
{
   std::vector< std::invoke_result_t< Predicate > > results;
   std::transform(container.begin(), container.end(), std::back_inserter(results), f);
   return container;
}

template < typename Predicate, typename Container, typename OutContainer >
inline OutContainer
apply(Predicate f, const Container& container, const OutContainer&& container_out)
{
   std::transform(container.begin(), container.end(), std::back_inserter(container_out), f);
   return container_out;
}

template < typename Predicate, typename Container >
inline void for_each(Predicate f, Container& container)
{
   std::for_each(container.begin(), container.end(), f);
}
template < typename Predicate, typename Container >
inline void for_each(Predicate f, const Container& container)
{
   std::for_each(container.begin(), container.end(), f);
}

template < typename VectorT, typename IndexVectorT >
inline void remove_by_sorted_indices(VectorT& v, const IndexVectorT& indices)
{
   static_assert(
      std::is_integral_v< typename IndexVectorT::value_type >,
      "Index vector value_type needs to be an integer type");
   std::for_each(indices.crbegin(), indices.crend(), [&v](auto index) {
      v.erase(std::next(begin(v), index));
   });
}
template < typename VectorT, typename IndexVectorT >
inline void remove_by_indices(VectorT& v, const IndexVectorT& indices)
{
   auto indices_copy = indices;
   std::sort(indices_copy.begin(), indices_copy.end());
   remove_by_sorted_indices(v, indices_copy);
}

}  // namespace algo

#endif  // LORAINE_ALGORITHMS_H
