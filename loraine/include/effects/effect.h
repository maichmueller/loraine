
#ifndef LORAINE_EFFECT_H
#define LORAINE_EFFECT_H

#include "core/gamedefs.h"
#include "core/targeting.h"
#include "events/event_listener.h"
#include "events/eventbase.h"
#include "utils/types.h"
#include "utils/utils.h"

// forward-declarations
class GameMode;
class Card;
class Controller;

/**
 * The base class for effects in the game.
 */

class EffectBase:
    public Cloneable< EffectBase >,
    public EventListener< EffectBase >,
    public Targeting {
  public:
   enum class Type { AOE = 0, AURA, SIMPLE, TARGETING };
   enum class RegistrationTime {
      CREATION = 0,
      DRAW,
      DEATH,
      SUMMON,
   };

   explicit EffectBase(
      sptr< Card > card_ptr,
      RegistrationTime reg_time,
      const sptr< TargeterBase >& targeter = std::make_shared< NoneTargeter >(),
      Type type = Type::SIMPLE)
       : Targeting(targeter),
         m_effect_type(type),
         m_reg_time(reg_time),
         m_assoc_card(std::move(card_ptr)),
         m_uuid(utils::new_uuid())
   {
   }
   EffectBase(const EffectBase& effect);
   EffectBase(EffectBase&& effect) noexcept = default;
   EffectBase& operator=(EffectBase&& rhs) = default;
   EffectBase& operator=(const EffectBase& rhs) = default;
   ~EffectBase() override = default;

   [[nodiscard]] bool is_consumed() const { return m_consumed; }
   inline void consume() { m_consumed = true; }

   void associated_card(sptr< Card > card) { m_assoc_card = std::move(card); }
   [[nodiscard]] auto associated_card() const { return m_assoc_card; }
   [[nodiscard]] auto effect_type() const { return m_effect_type; }
   [[nodiscard]] auto registration_time() const { return m_reg_time; }

   [[nodiscard]] auto& uuid() const { return m_uuid; }

   bool operator==(const EffectBase& effect) const;
   bool operator!=(const EffectBase& effect) const;

  private:
   Type m_effect_type;
   RegistrationTime m_reg_time;
   bool m_consumed = false;
   sptr< Card > m_assoc_card;
   UUID m_uuid;
};

/**
 * Any concrete effect implementation has to
 * inherit from this base to be eligible as subscriber to the m_subscribed_events in the game.
 * @tparam Events
 */
template < typename... Events >
class Effect:
    public Cloneable< Effect< Events... >, inherit_constructors< EffectBase > >,
    public EventCallInterface< Events... > {
};

#endif  // LORAINE_EFFECT_H
