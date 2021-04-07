
#ifndef LORAINE_EFFECTMAP_H
#define LORAINE_EFFECTMAP_H

#include <array>

#include "effects/effect.h"

namespace detail {

/**
 * The following is meant to produce merely an automated creation of a tuple which contains all the
 * effect types associated to the event labels. This should create a tuple of the form:
 *      std::tuple< std::vector< sptr< AttackEffect > >,
 *                  std::vector< sptr< BeholdEffect > >,
 *                  ...,
 *                  std::vector< sptr< LastCurrentlySupportedEffect > >
 *                >;
 */

struct effect_tuple_builder {
   template < size_t... Ints >
   using effect_tuple = std::tuple< std::vector<
      sptr< helpers::eventlabel_to_effect_t< static_cast< events::EventLabel >(Ints) > > >... >;

   template < size_t... Ints >
   effect_tuple< Ints... > make_effect_tuple(std::index_sequence< Ints... >);
};

using EffectVectorTuple = decltype(std::declval< effect_tuple_builder >().make_effect_tuple(
   std::declval< std::make_index_sequence< events::n_events > >()));

}  // namespace detail

class EffectMap {

   /**
    * Setter method
    * @tparam e_type
    * @tparam EffectType
    * @param effects
    */
   template < events::EventLabel e_type, typename EffectType >
   void effects(
      std::tuple_element_t< static_cast< size_t >(e_type), detail::EffectVectorTuple > effects);

   template < events::EventLabel elabel >
   [[nodiscard]] inline auto& effects()
   {
      return std::get< static_cast< size_t >(elabel) >(m_effects);
   }
   template < events::EventLabel elabel >
   [[nodiscard]] inline auto& effects() const
   {
      return std::get< static_cast< size_t >(elabel) >(m_effects);
   }
   /**
    * Runtime decision of which effects to return
    * @param elabel
    * @return
    */
   [[nodiscard]] inline auto& effects(events::EventLabel elabel);
   [[nodiscard]] inline auto& effects(events::EventLabel elabel) const;

  private:
   detail::EffectVectorTuple m_effects = {};

   //   std::vector< sptr< effects::AttackEffect > > m_attack_effect{};
   //   std::vector< sptr< effects::BeholdEffect > > m_behold_effect{};
   //   std::vector< sptr< effects::BlockEffect > > m_block_effect{};
   //   std::vector< sptr< effects::CaptureEffect > > m_capture_effect{};
   //   std::vector< sptr< effects::CastEffect > > m_cast_effect{};
   //   std::vector< sptr< effects::DaybreakEffect > > m_daybreak_effect{};
   //   std::vector< sptr< effects::DiscardEffect > > m_discard_effect{};
   //   std::vector< sptr< effects::DrawCardEffect > > m_drawcard_effect{};
   //   std::vector< sptr< effects::EnlightenmentEffect > > m_englightenment_effect{};
   //   std::vector< sptr< effects::GainManagemEffect > > m_gainmanagem_effect{};
   //   std::vector< sptr< effects::HealUnitEffect > > m_healunit_effect{};
   //   std::vector< sptr< effects::LevelUpEffect > > m_levelup_effect{};
   //   std::vector< sptr< effects::NexusDamageEffect > > m_nexusdamage_effect{};
   //   std::vector< sptr< effects::NexusStrikeEffect > > m_nexuststrike_effect{};
   //   std::vector< sptr< effects::NightfallEffect > > m_nightfall_effect{};
   //   std::vector< sptr< effects::PlayEffect > > m_play_effect{};
   //   std::vector< sptr< effects::RecallEffect > > m_recall_effect{};
   //   std::vector< sptr< effects::RoundEndEffect > > m_roundend_effect{};
   //   std::vector< sptr< effects::RoundStartEffect > > m_roundstart_effect{};
   //   std::vector< sptr< effects::ScoutEffect > > m_scout_effect{};
   //   std::vector< sptr< effects::SlayEffect > > m_slay_effect{};
   //   std::vector< sptr< effects::StrikeEffect > > m_strike_effect{};
   //   std::vector< sptr< effects::StunEffect > > m_stun_effect{};
   //   std::vector< sptr< effects::SummonEffect > > m_summon_effect{};
   //   std::vector< sptr< effects::SupportEffect > > m_support_effect{};
   //   std::vector< sptr< effects::TargetEffect > > m_target_effect{};
   //   std::vector< sptr< effects::UnitDamageEffect > > m_unitdamage_effect{};
};

#endif  // LORAINE_EFFECTMAP_H
