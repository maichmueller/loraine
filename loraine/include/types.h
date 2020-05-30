//
// Created by michael on 22.05.20.
//

#ifndef LORAINE_TYPES_H
#define LORAINE_TYPES_H

#include <cinttypes>
#include <memory>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
// short ID
using SID = u8;
// long ID
using LID = u64;
// Card ID
using CardID = u64;
// short handles for most common pointer types
template < typename T >
using uptr = std::unique_ptr< T >;
template < typename T >
using sptr = std::shared_ptr< T >;
template < typename T >
using wptr = std::weak_ptr< T >;
// a symmetric array to be used to store player symmetric concepts
template <typename T>
using SymArray = std::array<T, 2>;


#endif  // LORAINE_TYPES_H
