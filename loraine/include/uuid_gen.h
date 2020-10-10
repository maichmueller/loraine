
#ifndef LORAINE_UUID_GEN_H
#define LORAINE_UUID_GEN_H

#include <uuid.h>
#include "types.h"

static auto
   generator = uuids::uuid_random_generator();

inline UUID new_uuid()
{
   return generator();
}

#endif  // LORAINE_UUID_GEN_H
