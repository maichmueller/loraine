
#ifndef LORAINE_EFFECT_H
#define LORAINE_EFFECT_H

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
 * The base class for effects in the game.
 */
template < typename... Events >
class IEffectComponent:
    public Cloneable< IEffectComponent< Events... > >,
    public events::IEventSubscriber< Events... >,
    public Targeting {
  public:
   enum class Style { AOE = 0, AURA, SIMPLE, TARGETING };
   enum class RegistrationTime {
      CREATION = 0,
      DRAW,
      DEATH,
      SUMMON,
   };

   IEffectComponent(
      sptr< Card > card_ptr,
      RegistrationTime reg_time,
      const sptr< TargeterBase >& targeter = std::make_shared< NoneTargeter >(),
      Style style = Style::SIMPLE)
       : Targeting(targeter),
         m_style(style),
         m_reg_time(reg_time),
         m_assoc_card(std::move(card_ptr)),
         m_uuid(utils::new_uuid())
   {
   }
   IEffectComponent(const IEffectComponent& effect);
   IEffectComponent(IEffectComponent&& effect) noexcept = default;
   IEffectComponent& operator=(IEffectComponent&& rhs) = default;
   IEffectComponent& operator=(const IEffectComponent& rhs) = default;
   ~IEffectComponent() override = default;

   virtual void operator()(GameState& state) = 0;
   virtual bool is_composite() = 0;

   bool requires_targeting() const { return m_style == Style::TARGETING; }

   [[nodiscard]] bool is_consumed() const { return m_consumed; }
   inline void consume() { m_consumed = true; }

   void associated_card(sptr< Card > card) { m_assoc_card = std::move(card); }
   [[nodiscard]] auto associated_card() const { return m_assoc_card; }
   [[nodiscard]] auto label() const { return m_style; }
   [[nodiscard]] auto registration_time() const { return m_reg_time; }

   [[nodiscard]] inline constexpr auto subscribes_to() const
   {
      return std::initializer_list< events::EventLabel >{Events::label...};
   }

   [[nodiscard]] auto& uuid() const { return m_uuid; }

   bool operator==(const IEffectComponent& effect) const;
   bool operator!=(const IEffectComponent& effect) const;

  private:
   Style m_style;
   RegistrationTime m_reg_time;
   bool m_consumed = false;
   sptr< Card > m_assoc_card;
   UUID m_uuid;
};

template < typename... Events >
class EffectLeaf: public IEffectComponent< Events... > {

   bool is_composite() const override { return false; }
};

template < typename... Events >
class EffectComposite: public IEffectComponent< Events... > {
   bool is_composite() const override { return true; }

  private:
   std::vector< IEffectComponent< Events... >* > m_constituents;
};

using IAttackEffectComponent = IEffectComponent< events::AttackEvent >;
using IBeholdEffectComponent = IEffectComponent< events::BeholdEvent >;
using IBlockEffectComponent = IEffectComponent< events::BlockEvent >;
using ICaptureEffectComponent = IEffectComponent< events::CaptureEvent >;
using ICastEffectComponent = IEffectComponent< events::CastEvent >;
using IDaybreakEffectComponent = IEffectComponent< events::DaybreakEvent >;
using IDiscardEffectComponent = IEffectComponent< events::DiscardEvent >;
using IDrawCardEffectComponent = IEffectComponent< events::DrawCardEvent >;
using IEnlightenmentEffectComponent = IEffectComponent< events::EnlightenmentEvent >;
using IGainManagemEffectComponent = IEffectComponent< events::GainManagemEvent >;
using IHealUnitEffectComponent = IEffectComponent< events::HealUnitEvent >;
using ILevelUpEffectComponent = IEffectComponent< events::LevelUpEvent >;
using INexusDamageEffectComponent = IEffectComponent< events::NexusDamageEvent >;
using INexusStrikeEffectComponent = IEffectComponent< events::NexusStrikeEvent >;
using INightfallEffectComponent = IEffectComponent< events::NightfallEvent >;
using IPlayEffectComponent = IEffectComponent< events::PlayEvent >;
using IRecallEffectComponent = IEffectComponent< events::RecallEvent >;
using IRoundEndEffectComponent = IEffectComponent< events::RoundEndEvent >;
using IRoundStartEffectComponent = IEffectComponent< events::RoundStartEvent >;
using IScoutEffectComponent = IEffectComponent< events::ScoutEvent >;
using ISlayEffectComponent = IEffectComponent< events::SlayEvent >;
using IStrikeEffectComponent = IEffectComponent< events::StrikeEvent >;
using IStunEffectComponent = IEffectComponent< events::StunEvent >;
using ISummonEffectComponent = IEffectComponent< events::SummonEvent >;
using ISupportEffectComponent = IEffectComponent< events::SupportEvent >;
using ITargetEffectComponent = IEffectComponent< events::TargetEvent >;
using IUnitDamageEffectComponent = IEffectComponent< events::UnitDamageEvent >;

}  // namespace effects

#endif  // LORAINE_EFFECT_H
