#include "rng_machine.h"

std::mt19937_64 rng::rng_default_engine{std::random_device()()};