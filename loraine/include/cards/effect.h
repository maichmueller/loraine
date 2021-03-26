
#ifndef LORAINE_EFFECT_H
#define LORAINE_EFFECT_H

#include "core/gamedefs.h"
#include "core/targetable.h"
#include "events/eventbase.h"
#include "targeter.h"
#include "utils/types.h"
#include "utils/utils.h"

// forward-declarations
class GameMode;
class Card;
class Controller;

/**
 * The base class for effects in the game.
 */

class EffectBase: public Cloneable< EffectBase > {
  public:
   enum class Type { AOE = 0, AURA, SIMPLE, TARGETING };

   explicit EffectBase(
      sptr< Card > card_ptr,
      const sptr< TargeterBase >& targeter = std::make_shared< NoneTargeter >(),
      Type type = Type::SIMPLE)
       : m_effect_type(type),
         m_assoc_card(std::move(card_ptr)),
         m_targeter(targeter),
         m_targets(),
         m_uuid(utils::new_uuid())
   {
   }
   EffectBase(const EffectBase& effect) = default;
   EffectBase(EffectBase&& effect) noexcept = default;
   EffectBase& operator=(EffectBase&& rhs) = default;
   EffectBase& operator=(const EffectBase& rhs) = default;

   virtual ~EffectBase() = default;

   inline void targets(std::vector< sptr< Targetable > > targets)
   {
      m_targets = std::move(targets);
   }
   [[nodiscard]] bool has_targets() const { return not m_targets.empty(); }
   [[nodiscard]] auto& targets() { return m_targets; }

   [[nodiscard]] auto targets() const { return m_targets; }
   inline void reset_targets() { m_targets.clear(); }
   [[nodiscard]] auto targeter() const { return m_targeter; }

   [[nodiscard]] auto& targeter() { return m_targeter; }

   [[nodiscard]] bool is_consumed() const { return m_consumed; }
   inline void consume() { m_consumed = true; }

   [[nodiscard]] auto associated_card() const { return m_assoc_card; }
   [[nodiscard]] auto effect_type() const { return m_effect_type; }

   [[nodiscard]] auto& uuid() const { return m_uuid; }

   bool operator==(const EffectBase& effect) const;
   bool operator!=(const EffectBase& effect) const;

  private:
   Type m_effect_type;
   bool m_consumed = false;
   sptr< Card > m_assoc_card;
   UUID m_uuid;
   sptr< TargeterBase > m_targeter;
   std::vector< sptr< Targetable > > m_targets;
};

/**
 * Any concrete effect implementation has to
 * inherit from this base to be eligible as subscriber to the events in the game.
 * @tparam Events
 */
template < typename... Events >
class Effect:
    public Cloneable< Effect< Events... >, EffectBase >,
    public EventCallInterface< Events... > {
};

#endif  // LORAINE_EFFECT_H
