
#ifndef LORAINE_EFFECT_H
#define LORAINE_EFFECT_H

#include "core/gamedefs.h"
#include "events/eventbase.h"
#include "targeter.h"
#include "utils/types.h"
#include "utils/utils.h"
#include "core/targetable.h"

// forward-declarations
class GameMode;
class Card;
class Controller;

class EffectBase {
  public:
   explicit EffectBase(sptr< TargeterBase > targeter) : m_targeter(std::move(targeter)), m_targets() {}
   virtual ~EffectBase() = default;
   virtual bool operator==(const EffectBase& other) = 0;

   inline void targets(std::vector< sptr<Targetable> > targets) { m_targets = std::move(targets); }
   [[nodiscard]] bool has_targets() const { return not m_targets.empty(); }
   [[nodiscard]] auto& targets() { return m_targets; }
   [[nodiscard]] auto targets() const { return m_targets; }

   inline void reset_targets() { m_targets.clear(); }
   [[nodiscard]] auto targeter() const { return m_targeter; }
   [[nodiscard]] auto& targeter() { return m_targeter; }

  private:
   sptr< TargeterBase > m_targeter;
   std::vector< sptr<Targetable> > m_targets;
};

/**
 * The Abstract base class for effects in the game. Any concrete effect implementation has to
 * inherit from this base to be eligible as subscriber to the events in the game.
 * @tparam Events
 */

template < typename... Events >
class Effect: public EffectBase, public EventCallInterface< Events... > {
  public:
   enum class Type { AOE = 0, AURA, SIMPLE, TARGETING };

   [[nodiscard]] virtual bool target(const State& state, Controller& controller, Team team);
   [[nodiscard]] bool is_consumed() const { return m_consumed; }
   inline void consume() { m_consumed = true; }

   [[nodiscard]] auto associated_card() const { return m_assoc_card; }
   [[nodiscard]] auto effect_type() const { return m_effect_type; }


   [[nodiscard]] auto& uuid() { return m_uuid; }

   bool operator==(const Effect& effect) const;
   bool operator!=(const Effect& effect) const;

   Effect(
      sptr< Card > card_ptr,
      const sptr< TargeterBase >& targeter = std::make_shared< NoneTargeter >())
       : EffectBase(targeter),
         m_assoc_card(std::move(card_ptr)),
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
