
#ifndef LORAINE_SYSTEM_H
#define LORAINE_SYSTEM_H

#include "entt/entt.hpp"

class GameState;

/**
 * @brief Base class for continuous time updatable systems
 */
class IContinuousSystem {
  public:
   virtual void update(double deltatime) = 0;

  protected:
   IContinuousSystem(entt::registry& registry) : m_registry(registry) {}

   entt::registry& m_registry;
};


/**
 * @brief Base class for discrete time updatable systems (no elapsed time input needed)
 */
class IDiscreteSystem {
  public:
   virtual void update(GameState& state) = 0;

  protected:
   IDiscreteSystem(entt::registry& registry) : m_registry(registry) {}

   entt::registry& m_registry;
};


/**
 * @brief Base class for logic offering systems only. No update method implied
 */
class ILogicSystem {
  protected:
   ILogicSystem(entt::registry& registry) : m_registry(registry) {}

   entt::registry& m_registry;
};

#endif  // LORAINE_SYSTEM_H
