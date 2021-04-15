

#ifndef LORAINE_ALGORITHMS_H
#define LORAINE_ALGORITHMS_H

#include <algorithm>
#include <utility>

#include "utils/types.h"
#include "utils/utils.h"

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
template < typename Container, typename T >
auto find_index(const Container& container, const T& elem) -> long
{
   long index = -1;
   for(size_t i = 0; i < container.size(); ++i) {
      if(container[i] == elem) {
         index = i;
         break;
      }
   }
   return index;
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
auto copy_if(const Container& container, const FilterFunc& filter) -> std::vector< T >
{
   std::vector< T > elements;
   std::copy_if(container.begin(), container.end(), std::back_inserter(elements), filter);
   shuffle_inplace(container);
   return elements;
}

template < typename Container, typename T >
void remove_element(Container& container, const T& elem)
{
   auto index = algo::find_index(container, elem);
   if(index == -1) {
      throw std::logic_error("Element to remove was not found in ContainerType.");
   }
   container.erase(container.begin() + index);
}

template < class InputIt, class UnaryPredicate >
constexpr bool any_of(InputIt first, InputIt last, UnaryPredicate p)
{
   return std::find_if(first, last, p) != last;
}

template < class Container, class UnaryPredicate >
constexpr bool any_of(const Container& container, UnaryPredicate p)
{
   return std::find_if(container.begin(), container.end(), p) != container.end();
}

template < class InputIt, class UnaryPredicate >
constexpr bool all_of(InputIt first, InputIt last, UnaryPredicate p)
{
   return std::find_if_not(first, last, p) == last;
}

template < class Container, class UnaryPredicate >
constexpr bool all_of(const Container& container, UnaryPredicate p)
{
   return std::find_if_not(container.begin(), container.end(), p) == container.end();
}

template < class InputIt, class UnaryPredicate >
constexpr bool none_of(InputIt first, InputIt last, UnaryPredicate p)
{
   return not any_of(first, last, p);
}

template < class Container, class UnaryPredicate >
constexpr bool none_of(const Container& container, UnaryPredicate p)
{
   return not any_of(container, p);
}

template < typename Predicate, typename Container >
Container transform(Predicate f, Container& container)
{
   std::transform(container.begin(), container.end(), container.begin(), f);
   return container;
}
template < typename Predicate, typename Container, typename OutIter >
Container transform(Predicate f, const Container& container, OutIter out_iter)
{
   std::transform(container.begin(), container.end(), out_iter, f);
   return container;
}

template < typename Predicate, typename Container >
void for_each(Predicate f, Container& container)
{
   std::for_each(container.begin(), container.end(), f);
}
template < typename Predicate, typename Container >
void for_each(Predicate f, const Container& container)
{
   std::for_each(container.begin(), container.end(), f);
}

template < typename VectorT, typename IndexVectorT >
void remove_by_sorted_indices(VectorT& v, const IndexVectorT& indices)
{
   static_assert(
      std::is_integral_v< typename IndexVectorT::value_type >,
      "Index vector value_type needs to be an integer type");
   std::for_each(indices.crbegin(), indices.crend(), [&v](auto index) {
      v.erase(std::next(begin(v), index));
   });
}
template < typename VectorT, typename IndexVectorT >
void remove_by_indices(VectorT& v, const IndexVectorT& indices)
{
   auto indices_copy = indices;
   std::sort(indices_copy.begin(), indices_copy.end());
   remove_by_sorted_indices(v, indices_copy);
}

}  // namespace algo

#endif  // LORAINE_ALGORITHMS_H
