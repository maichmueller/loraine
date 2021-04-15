
#ifndef LORAINE_EFFECT_H
#define LORAINE_EFFECT_H

#include <entt/entt.hpp>

#include "core/gamedefs.h"
#include "core/targeting.h"
#include "events/event_listener.h"
#include "events/event_subscriber.h"
#include "events/event_types.h"
#include "utils/types.h"
#include "utils/utils.h"

// forward-declarations
class GameMode;
class Card;
class Controller;

namespace effects {

/**
 * The base class for add_effects in the game.
 */
template < typename... Events >
class IGameEffect: public events::IGameEventListener< Events... > {
  public:
   enum class Style { AOE = 0, AURA, SIMPLE, TARGETING };
   enum class RegistrationTime {
      CREATION = 0,
      DRAW,
      DEATH,
      SUMMON,
   };

   IGameEffect(entt::entity owner, RegistrationTime reg_time, Style style = Style::SIMPLE)
       : m_owner(owner), m_style(style), m_reg_time(reg_time)
   {
   }
   IGameEffect(const IGameEffect& effect) = default;
   IGameEffect(IGameEffect&& effect) noexcept = default;
   IGameEffect& operator=(IGameEffect&& rhs) = default;
   IGameEffect& operator=(const IGameEffect& rhs) = default;
   virtual ~IGameEffect() = default;

   virtual void operator()(GameState& state) = 0;
   virtual bool is_composite() = 0;

   bool requires_targeting() const { return m_style == Style::TARGETING; }

   [[nodiscard]] bool is_consumed() const { return m_consumed; }
   inline void consume() { m_consumed = true; }

   [[nodiscard]] auto owner(entt::entity owner) const { return m_owner; }
   [[nodiscard]] auto label() const { return m_style; }
   [[nodiscard]] auto registration_time() const { return m_reg_time; }

   [[nodiscard]] inline constexpr auto subscribes_to() const
   {
      return std::initializer_list< events::EventLabel >{Events::label...};
   }

   bool operator==(const IGameEffect& effect) const;
   bool operator!=(const IGameEffect& effect) const;

  private:
   entt::entity m_owner;
   Style m_style;
   RegistrationTime m_reg_time;
   bool m_consumed = false;
};

template < typename... Events >
class GameEffectLeaf: public IGameEffect< Events... > {
   bool is_composite() const override { return false; }
};

template < typename... Events >
class GameEffectComposite: public IGameEffect< Events... > {
   bool is_composite() const override { return true; }

  private:
   std::vector< IGameEffect< Events... >* > m_constituents;
};

}  // namespace effects

#endif  // LORAINE_EFFECT_H
