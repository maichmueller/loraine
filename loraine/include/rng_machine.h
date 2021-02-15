
#ifndef LORAINE_RNG_MACHINE_H
#define LORAINE_RNG_MACHINE_H

#include <random>

class rng {
  public:
   static auto& engine() {return rng_default_engine;}
  private:
   static std::mt19937_64 rng_default_engine;
};

#endif  // LORAINE_RNG_MACHINE_H
