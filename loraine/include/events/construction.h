
#ifndef LORAINE_CONSTRUCTION_H
#define LORAINE_CONSTRUCTION_H

#include "effects/effect.h"
#include "event_types.h"

//namespace events {
//
//namespace detail {

//template < EventLabel event_label >
//constexpr auto create_event()
//{
//   return helpers::label_to_event_t< event_label >();
//}
//
//template < size_t e >
//constexpr auto create_event()
//{
//   return helpers::label_to_event_t< static_cast< events::EventLabel >(e) >();
//}
//
//template < size_t... I >
//std::array< events::LOREvent, events::n_events > build_event_array_impl(std::index_sequence<I...> )
//{
//   return {events::LOREvent(create_event<I>())...};
//}
//
//}  // namespace detail
//
//inline std::array< events::LOREvent, events::n_events > build_event_array()
//{
//   return detail::build_event_array_impl(std::make_index_sequence< events::n_events >{});
//}

//}  // namespace events
#endif  // LORAINE_CONSTRUCTION_H
