
#ifndef LORAINE_EFFECT_H
#define LORAINE_EFFECT_H

#include <loraine/events/event_id.h>

#include <entt/entt.hpp>

#include "loraine/core/gamedefs.h"
#include "loraine/effects/effectdefs.h"
#include "loraine/events/event_subscriber.h"
#include "loraine/utils/types.h"
#include "loraine/utils/utils.h"

/**
 * The base class for effects in the game.
 */
template < typename... Events >
class IGameEffect: public events::IGameEventListener< IGameEffect< Events... >, Events... > {
  public:
   IGameEffect(entt::entity owner, RegistrationTime reg_time, Style style)
       : m_owner(owner), m_style(style), m_reg_time(reg_time)
   {
   }
   IGameEffect(const IGameEffect& effect) = default;
   IGameEffect(IGameEffect&& effect) noexcept = default;
   IGameEffect& operator=(IGameEffect&& rhs) = default;
   IGameEffect& operator=(const IGameEffect& rhs) = default;
   virtual ~IGameEffect() = default;

   virtual void operator()(GameState& state) = 0;
   virtual void add(uptr< IGameEffect >&& effect) = 0;
   virtual void remove(IGameEffect* effect) = 0;
   virtual bool is_composite() const { return false; }

   /**
    * @brief Updates the state of this effect
    *
    * Some effects hold conditions for their execution, which need to be updated.
    *
    * @param state
    * @return
    */
   virtual bool update(GameState& state) { return true; }

   bool is_targeting() const { return m_style == Style::TARGETING; }

   [[nodiscard]] bool is_consumed() const { return m_consumed; }
   inline void consume() { m_consumed = true; }

   [[nodiscard]] auto owner(entt::entity owner) const { return m_owner; }
   [[nodiscard]] auto style() const { return m_style; }
   [[nodiscard]] auto registration_time() const { return m_reg_time; }
   [[nodiscard]] inline constexpr static auto ids_of_interest()
   {
      return std::array< events::EventID, sizeof...(Events) >{Events::id...};
   }

   bool operator==(const IGameEffect& effect) const;
   bool operator!=(const IGameEffect& effect) const;

  private:
   entt::entity m_owner;
   RegistrationTime m_reg_time;
   Style m_style;
   bool m_consumed = false;
};
template < typename... Events >
class GameEffectLeaf: public IGameEffect< Events... > {
   using base = IGameEffect< Events... >;

   virtual void add(uptr< base >&& effect) override{};
   virtual void remove(base* effect) override{};
};
template < typename... Events >
class GameEffectComposite: public IGameEffect< Events... > {
   using base = IGameEffect< Events... >;

   virtual void add(uptr< base >&& effect) override
   {
      m_constituents.emplace_back(std::move(effect));
   };
   virtual void remove(base* effect) override{};
   bool is_composite() const override { return true; }

  private:
   std::vector< uptr< IGameEffect< Events... > > > m_constituents{};
};

#endif  // LORAINE_EFFECT_H
