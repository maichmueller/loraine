
#ifndef LORAINE_SYSTEM_HPP
#define LORAINE_SYSTEM_HPP

#include "entt/entt.hpp"

class GameState;

class System {
   // only the GameState is meant to change the context of the system, so as to not break coupling
   friend GameState;

  protected:
   GameState* m_gamestate = nullptr;
   entt::registry* m_registry = nullptr;

  private:
   /** @brief Sets the context for the system (the associated GameState).
    *
    * The gamestate is meant to call this function after creation of itself (i.e. when `this` is
    * accessible) to decide the context of this system. It could be also changed later on
    * (potentially unsafe).
    */
   void context(GameState& state);
};

/**
 * @brief Base class for continuous time updatable systems
 */
class IContinuousSystem: public System {
  public:
   virtual ~IContinuousSystem() = default;

   virtual void update(double deltatime) = 0;
};

/**
 * @brief Base class for discrete time updatable systems (no elapsed time input needed)
 */
class IDiscreteSystem: public System {
  public:
   virtual ~IDiscreteSystem() = default;

   virtual void update(GameState& state) = 0;
};

/**
 * @brief Base class for per round updated systems. Provides methods for round begin and end.
 */
class IRoundSystem: public System {
  public:
   virtual ~IRoundSystem() = default;

   virtual void update_begin(){};
   virtual void update_end(){};
};

/**
 * @brief Base class for logic offering systems only. No update method implied
 */
class ILogicSystem: public System {
};

#endif  // LORAINE_SYSTEM_HPP
