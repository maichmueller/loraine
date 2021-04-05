
#ifndef LORAINE_CONSTRUCTION_H
#define LORAINE_CONSTRUCTION_H

#include "effects/effect.h"
#include "event_types.h"

namespace events {

template < EventLabel event_label >
constexpr auto create_event()
{
   return label_to_event_helper_t< event_label >();
}

template < size_t e >
inline void fill_event_array(std::array< events::LOREvent, n_events >& arr)
{
   arr[e] = create_event< static_cast< EventLabel >(e) >();
   fill_event_array< e - 1 >(arr);
}

template <>
inline void fill_event_array< 0 >(std::array< events::LOREvent, n_events >& arr)
{
   arr[0] = create_event< static_cast< EventLabel >(0) >();
}

}  // namespace m_subscribed_events
#endif  // LORAINE_CONSTRUCTION_H
