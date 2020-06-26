
#ifndef LORAINE_UUID_GEN_H
#define LORAINE_UUID_GEN_H

#include <boost/uuid/uuid.hpp>  // uuid class
#include <boost/uuid/uuid_generators.hpp>  // generators
#include <boost/uuid/uuid_io.hpp>  // streaming operators etc.

#include "types.h"

static boost::uuids::random_generator
   generator = boost::uuids::random_generator();

UUID new_uuid()
{
   return generator();
}

#endif  // LORAINE_UUID_GEN_H
