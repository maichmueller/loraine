
#ifndef LORAINE_FILTER_H
#define LORAINE_FILTER_H

#include <functional>

#include "core/targetable.h"

using Filter = std::function< bool(const Targetable& /*target*/) >;

#endif  // LORAINE_FILTER_H
