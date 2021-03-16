
#ifndef LORAINE_CONSTRUCTION_H
#define LORAINE_CONSTRUCTION_H

#include "cards/effect.h"
#include "event_types.h"

namespace events {

template < EventLabel event_label >
constexpr auto create_event()
{
   constexpr auto idx = static_cast<size_t>(event_label);
   return utils::variant_element_t<idx, LOREvent>();
}


template < size_t e >
void fill_event_array(std::array< LOREvent , n_events >& arr)
{
   arr[e] = create_event< static_cast< EventLabel >(e) >();
   fill_event_array< e - 1 >(arr);
}

template <>
void fill_event_array< 0 >(std::array< LOREvent, n_events >& arr)
{
   arr[0] = create_event< static_cast< EventLabel >(0) >();
}

}  // namespace events
#endif  // LORAINE_CONSTRUCTION_H
