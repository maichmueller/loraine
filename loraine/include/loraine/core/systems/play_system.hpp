

#ifndef LORAINE_PLAY_SYSTEM_HPP
#define LORAINE_PLAY_SYSTEM_HPP

#include <entt/entt.hpp>

#include "loraine/core/systems/system.hpp"

class SummonSystem: public System {
  public:
   SummonSystem() = default;

   void play();
   void summon();

   bool to_play_queue(entt::entity card);
   bool to_summon_queue(entt::entity card) { m_play_queue.emplace_back(card); }

  private:
   std::vector< entt::entity > m_play_queue;
   std::vector< entt::entity > m_summon_queue;
};

#endif  // LORAINE_PLAY_SYSTEM_HPP
