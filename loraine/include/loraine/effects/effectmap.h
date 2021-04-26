
#ifndef LORAINE_EFFECTMAP_H
#define LORAINE_EFFECTMAP_H

#include <array>

#include "entt/entt.hpp"
#include "loraine/core/systems/effect_system.hpp"
#include "loraine/events/event_id.h"
#include "loraine/events/helpers.h"

//namespace detail {
//
///**
// * The following is meant to produce merely an automated creation of a tuple which contains all the
// * effect types associated to the event labels. This should create a tuple of the form:
// *      std::tuple< std::vector< sptr< AttackEffect > >,
// *                  std::vector< sptr< BeholdEffect > >,
// *                  ...,
// *                  std::vector< sptr< LastCurrentlySupportedEffect > >
// *                >;
// */
//
//struct effect_tuple_builder {
//   template < size_t... Ints >
//   using effect_tuple = std::tuple< std::vector< sptr< events::IGameEventListener<
//      helpers::label_to_event_t< static_cast< events::EventLabel >(Ints) > > > >... >;
//
//   template < size_t... Ints >
//   effect_tuple< Ints... > make_effect_tuple(std::index_sequence< Ints... >);
//};
//
//using EffectVectorTuple = decltype(std::declval< effect_tuple_builder >().make_effect_tuple(
//   std::declval< std::make_index_sequence< events::n_events > >()));
//
//}  // namespace detail
//
//class EffectMap {
//  private:
//   template < events::EventLabel elabel >
//   using EffectContainerType = std::
//      tuple_element_t< static_cast< size_t >(elabel), detail::EffectVectorTuple >;
//   template < events::EventLabel elabel >
//   using IListener = events::IGameEventListener< helpers::label_to_event_t< elabel > >;
//
//   /**
//    * Setter method
//    * @tparam e_type
//    * @param effects
//    */
//   template <
//      events::EventLabel elabel,
//      typename Container,
//      typename = std::enable_if_t<
//         std::is_convertible_v< typename Container::value_type, sptr< IListener< elabel > > > > >
//   void add_effects(Container&& effects) {
//      effects<elabel>(m_effects).emplace_back(effects);
//   }
//
//   template <
//      events::EventLabel elabel,
//      typename Container,
//      typename = std::enable_if_t<
//         std::is_convertible_v< typename Container::value_type, sptr< IListener< elabel > > > > >
//   void remove_effects(Container effects);
//
//   template < typename EventType >
//   [[nodiscard]] inline auto& get()
//   {
//      return std::get< EventType >(m_effects);
//   }
//
//   template < events::EventLabel elabel >
//   [[nodiscard]] inline auto& get()
//   {
//      return std::get< static_cast< size_t >(elabel) >(m_effects);
//   }
//   template < typename EventType >
//   [[nodiscard]] inline auto& get() const
//   {
//      return std::get< EventType >(m_effects);
//   }
//   template < events::EventLabel elabel >
//   [[nodiscard]] inline auto& get() const
//   {
//      return std::get< static_cast< size_t >(elabel) >(m_effects);
//   }
//
//  private:
//   detail::EffectVectorTuple m_effects = {};
//
//   //   std::vector< sptr< get::AttackEffect > > m_attack_effect{};
//   //   std::vector< sptr< get::BeholdEffect > > m_behold_effect{};
//   //   std::vector< sptr< get::BlockEffect > > m_block_effect{};
//   //   std::vector< sptr< get::CaptureEffect > > m_capture_effect{};
//   //   std::vector< sptr< get::CastEffect > > m_cast_effect{};
//   //   std::vector< sptr< get::DaybreakEffect > > m_daybreak_effect{};
//   //   std::vector< sptr< get::DiscardEffect > > m_discard_effect{};
//   //   std::vector< sptr< get::DrawCardEffect > > m_drawcard_effect{};
//   //   std::vector< sptr< get::EnlightenmentEffect > > m_englightenment_effect{};
//   //   std::vector< sptr< get::GainManagemEffect > > m_gainmanagem_effect{};
//   //   std::vector< sptr< get::HealUnitEffect > > m_healunit_effect{};
//   //   std::vector< sptr< get::LevelUpEffect > > m_levelup_effect{};
//   //   std::vector< sptr< get::NexusDamageEffect > > m_nexusdamage_effect{};
//   //   std::vector< sptr< get::NexusStrikeEffect > > m_nexuststrike_effect{};
//   //   std::vector< sptr< get::NightfallEffect > > m_nightfall_effect{};
//   //   std::vector< sptr< get::PlayEffect > > m_play_effect{};
//   //   std::vector< sptr< get::RecallEffect > > m_recall_effect{};
//   //   std::vector< sptr< get::RoundEndEffect > > m_roundend_effect{};
//   //   std::vector< sptr< get::RoundStartEffect > > m_roundstart_effect{};
//   //   std::vector< sptr< get::ScoutEffect > > m_scout_effect{};
//   //   std::vector< sptr< get::SlayEffect > > m_slay_effect{};
//   //   std::vector< sptr< get::StrikeEffect > > m_strike_effect{};
//   //   std::vector< sptr< get::StunEffect > > m_stun_effect{};
//   //   std::vector< sptr< get::SummonEffect > > m_summon_effect{};
//   //   std::vector< sptr< get::SupportEffect > > m_support_effect{};
//   //   std::vector< sptr< get::TargetEffect > > m_target_effect{};
//   //   std::vector< sptr< get::UnitDamageEffect > > m_unitdamage_effect{};
//};
//
//template < events::EventLabel e_type >
//void EffectMap::add_effects(effects)
//{
//
//}

#endif  // LORAINE_EFFECTMAP_H
