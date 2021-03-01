
#ifndef LORAINE_EFFECT_H
#define LORAINE_EFFECT_H

#include <any>
#include <functional>
#include <utility>

#include "engine/gamedefs.h"
#include "events/lor_events/all_events.h"
#include "targeter.h"
#include "utils/types.h"
#include "utils/utils.h"

// forward-declarations
class GameMode;
class Card;
class Target;
class Controller;

class EffectBase {
  public:
   virtual ~EffectBase() = default;
   virtual bool operator==(const EffectBase& other) = 0;
};

template < typename... Events >
class Effect:
    public EffectBase,
    public EventCallInterface< Events... > {
  public:
   enum class Type { AOE = 0, AURA, SIMPLE, TARGETING };

   [[nodiscard]] virtual bool target(const State& state, Controller& controller, Team team);
   [[nodiscard]] bool is_consumed() const { return m_consumed; }
   inline void consume() { m_consumed = true; }

   [[nodiscard]] auto associated_card() const { return m_assoc_card; }
   [[nodiscard]] auto effect_type() const { return m_effect_type; }

   inline void targets(std::vector< Target > targets) { m_targets = std::move(targets); }
   [[nodiscard]] bool has_targets() const { return not m_targets.empty(); }
   [[nodiscard]] auto& targets() { return m_targets; }
   [[nodiscard]] auto targets() const { return m_targets; }

   inline void reset_targets() { m_targets.clear(); }
   [[nodiscard]] auto targeter() const { return m_targeter; }
   [[nodiscard]] auto& targeter() { return m_targeter; }

   [[nodiscard]] auto& uuid() { return m_uuid; }

   bool operator==(const Effect& effect) const;
   bool operator!=(const Effect& effect) const;

   Effect(
      sptr< Card > card_ptr,
      sptr< TargeterBase > targeter = std::make_shared< NoneTargeter >(),
      Type effect_type = Type::SIMPLE)
       : m_effect_type(effect_type),
         m_assoc_card(std::move(card_ptr)),
         m_targeter(std::move(targeter)),
         m_uuid(utils::new_uuid())
   {
   }
   Effect(const Effect& effect) = default;
   Effect(Effect&& effect) noexcept = default;
   Effect& operator=(Effect&& rhs) = default;
   Effect& operator=(const Effect& rhs) = default;

  private:
   Type m_effect_type;
   bool m_consumed = false;
   sptr< Card > m_assoc_card;
   sptr< TargeterBase > m_targeter;
   std::vector< Target > m_targets;
   UUID m_uuid;

};

template < typename... Events >
bool Effect< Events... >::operator==(const Effect& effect) const
{
   return m_uuid == effect.uuid();
}
template < typename... Events >
bool Effect< Events... >::operator!=(const Effect& effect) const
{
   return not (*this == effect);
}


#endif  // LORAINE_EFFECT_H
