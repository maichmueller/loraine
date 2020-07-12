//
// Created by michael on 24.06.20.
//

#ifndef LORAINE_RNG_MACHINE_H
#define LORAINE_RNG_MACHINE_H

#include <random>

namespace rng {
   static std::random_device rng_dev;
   static std::mt19937_64 rng_def_engine{rng_dev()};
}

#endif  // LORAINE_RNG_MACHINE_H
