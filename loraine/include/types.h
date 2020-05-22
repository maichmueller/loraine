//
// Created by michael on 22.05.20.
//

#ifndef LORAINE_TYPES_H
#define LORAINE_TYPES_H

#include <cinttypes>
#include <memory>

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

using ID = uint64;

template <typename T>
using uptr = std::unique_ptr<T>;

template <typename T>
using sptr = std::shared_ptr<T>;

template <typename T>
using wptr = std::weak_ptr<T>;

#endif  // LORAINE_TYPES_H
