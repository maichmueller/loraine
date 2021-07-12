

#ifndef LORAINE_PLAY_SYSTEM_HPP
#define LORAINE_PLAY_SYSTEM_HPP

#include <entt/entt.hpp>

#include "loraine/core/systems/system.hpp"

class PlaySystem: public System {
  public:
   PlaySystem() = default;

   void play(bool latest_only);
   void summon();

   void to_play_queue(entt::entity card) { m_play_queue.emplace_back(card); }
   void to_summon_queue(entt::entity card) { m_summon_queue.emplace_back(card); }

  private:
   std::vector< entt::entity > m_play_queue;
   std::vector< entt::entity > m_summon_queue;
};

#endif  // LORAINE_PLAY_SYSTEM_HPP
