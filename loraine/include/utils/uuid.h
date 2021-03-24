
#ifndef LORAINE_UUID_H
#define LORAINE_UUID_H

#include <array>
#include <mutex>
#include <set>

namespace uuids {

class Pool {
  public:
   using int_type = uint64_t;

   explicit Pool(int_type max_value = std::numeric_limits< int_type >::max()) : m_max(max_value) {}

   int_type get() { return m_counter++; }

  private:
   int_type m_counter = 0;
   int_type m_max;
};

class UUID {
   explicit UUID(Pool& pool) : m_pool(&pool), m_value(pool.get()) {}

  private:
   uint64_t m_value;
   Pool* m_pool;
};

}  // namespace uuids

#endif  // LORAINE_UUID_H
