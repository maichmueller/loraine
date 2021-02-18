

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
auto find_indices(const Container& container, const std::vector<T>& elems) -> std::vector< long >
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

//template < typename Container, typename T, typename FilterFunc >
//auto pop_index_r(const Container& container, std::vector<size_t> indices) -> std::vector< T >
//{
//   auto begin = container.begin();
//   // with every pop we need to calculate the new filtered indices as per
//   // shift by 1
//   size_t idx_shift = 0;
//
//   for(const auto& idx : indices) {
//      size_t shifted_idx = idx - idx_shift;
//      popped_elems.emplace_back(pop_by_index(container, shifted_idx));
//      ++idx_shift;
//   }
//   T popped_elem = container.at(std::distance(pos, begin) - 1);
//   container.erase(pos);
//   return {popped_elem};
//}

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

template <typename Container, typename T>
void remove_element(Container& container, const T& elem)
{
   auto index = algo::find_index(container, elem);
   if(index == -1) {
      throw std::logic_error("Element to remove was not found in container.");
   }
   container.erase(container.begin() + index);
}

}  // namespace algo

#endif  // LORAINE_ALGORITHMS_H
