
#ifndef LORAINE_RNG_H
#define LORAINE_RNG_H

#include <random>

struct rng {
   using rng_type = std::mt19937_64;
   using seed_type = rng_type::result_type;

   static std::mt19937_64 create(seed_type seed = std::random_device()())
   {
      return std::mt19937_64{seed};
   }
};

#endif  // LORAINE_RNG_H
