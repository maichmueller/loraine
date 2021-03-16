
#ifndef LORAINE_TYPES_H
#define LORAINE_TYPES_H

#include <cinttypes>
#include <memory>
#include <uuid.h>


using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

// unique universal identifier type
using UUID = uuids::uuid;
// short handles for most common pointer types
template < typename T >
using uptr = std::unique_ptr< T >;
template < typename T >
using sptr = std::shared_ptr< T >;
template < typename T >
using wptr = std::weak_ptr< T >;
// a symmetric array to be used to store team symmetric concepts
template <typename T>
using SymArr = std::array<T, 2>;


#endif  // LORAINE_TYPES_H
