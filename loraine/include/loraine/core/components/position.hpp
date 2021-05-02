

#ifndef LORAINE_POSITION_HPP
#define LORAINE_POSITION_HPP

#include "loraine/core/gamedefs.h"

template < Zone z>
struct Position {
   constexpr static auto zone = z;
   /// the index in the current zone
   size_t index;
};

#endif  // LORAINE_POSITION_HPP
