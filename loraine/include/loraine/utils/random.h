
#ifndef LORAINE_RANDOM_H
#define LORAINE_RANDOM_H

#include <algorithm>
#include <random>



struct random {
   using rng_type = std::mt19937_64;
   using seed_type = rng_type::result_type;

   static std::mt19937_64 create_rng(seed_type seed = std::random_device()())
   {
      return std::mt19937_64{seed};
   }

   template < typename Container, class RNG >
   static void shuffle_inplace(Container& container, RNG&& rng)
   {
      std::shuffle(container.begin(), container.end(), std::forward< RNG >(rng));
   }
   template < class RNG >
   static inline bool bernoulli_sample(double p, RNG&& rng)
   {
      std::bernoulli_distribution ber(p);
      return ber(std::forward< RNG >(rng));
   }

   template <
      class BiIter,
      class RNG,
      typename Distribution = std::uniform_int_distribution< uint64_t > >
   static void shuffle_inplace_limited(
      BiIter begin,
      BiIter end,
      size_t num_random,
      RNG&& rng,
      Distribution weight_dist = Distribution(0))
   {
      // Fisher-Yates-shuffle
      size_t N = std::distance(begin, end);
      while(num_random--) {
         BiIter r = begin;
         std::advance(r, weight_dist(std::forward< RNG >(rng)) % N);
         std::swap(*begin, *r);
         ++begin;
         --N;
      }
   }

   template < typename T, typename Allocator >
   static void choose_inplace(std::vector< T, Allocator >& vec, int n = 1)
   {
      shuffle_inplace_limited(vec.begin(), vec.end(), n);
   }
   template < typename T, typename Allocator >
   void choose_inplace(std::vector< T, Allocator >& vec, std::vector< double > weights, int n = 1)
   {
      shuffle_inplace_limited(
         vec.begin(),
         vec.end(),
         n,
         std::discrete_distribution< uint64_t >{weights.begin(), weights.end()});
   }
   template < typename T, typename Allocator, class RNG >
   static std::vector< T, Allocator >
   choose(const std::vector< T, Allocator >& vec, RNG&& rng, int n)
   {
      std::vector< size_t > indices(vec.size());
      std::iota(indices.begin(), indices.end(), 0);
      shuffle_inplace_limited(indices.begin(), indices.end(), n, std::forward< RNG >(rng));
      std::vector< T, Allocator > out;
      out.reserve(n);
      for(auto idx : indices) {
         out.emplace_back(vec[idx]);
      }
      return out;
   }
   template < typename T, typename Allocator, class RNG >
   static T&
   choose(const std::vector< T, Allocator >& vec, RNG& rng)
   {
      std::vector< size_t > indices(vec.size());
      std::iota(indices.begin(), indices.end(), 0);
      shuffle_inplace_limited(indices.begin(), indices.end(), 1, rng);
      return vec[indices[0]];
   }

};

#endif  // LORAINE_RANDOM_H
