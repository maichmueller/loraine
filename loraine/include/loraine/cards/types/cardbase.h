
#ifndef LORAINE_CARD_H
#define LORAINE_CARD_H

#include <loraine/cards/toll.h>

#include <entt/entt.hpp>
#include <map>
#include <set>
#include <utility>
#include <vector>

#include "loraine/cards/carddefs.h"
#include "loraine/core/targeting.h"
#include "loraine/effects/effect.h"
#include "loraine/events/event_labels.h"
#include "loraine/events/event_listener.h"
#include "loraine/events/event_subscriber.h"
#include "loraine/utils/algorithms.h"
#include "loraine/utils/types.h"
#include "loraine/utils/utils.h"

// forward-declarations

class GameState;
class Grant;



struct MutableState {
   uptr< Toll > play_toll = std::make_unique< Toll >();
   // all permanent grants
   std::vector< sptr< Grant > > grants = {};
   // all temporary grants
   std::vector< sptr< Grant > > grants_temp = {};
};
//
//[[nodiscard]] inline auto mana_cost() const {}
//void effects(events::EventLabel e_type, std::vector< sptr< IEffect > > effects);
//[[nodiscard]] inline auto& effects(events::EventLabel etype)
//{
//   return m_mutables.add_effects.at(etype);
//}
//[[nodiscard]] inline auto& effects(events::EventLabel etype) const
//{
//   return m_mutables.add_effects.at(etype);
//}
//[[nodiscard]] inline auto& effects()
//{
//   return m_mutables.add_effects;
//}
//[[nodiscard]] inline auto& effects() const
//{
//   return m_mutables.add_effects;
//}
//
//[[nodiscard]] auto creator() const
//{
//   return m_immutables.creator_code.value();
//}
//
//[[nodiscard]] std::vector< sptr< Grant > > all_grants() const;
//
//// status requests
//[[nodiscard]] inline bool is_spell() const
//{
//   return m_immutables.card_type == CardType::SPELL;
//}
//[[nodiscard]] inline bool is_unit() const
//{
//   return m_immutables.card_type == CardType::UNIT;
//}
//[[nodiscard]] inline bool is_landmark() const
//{
//   return m_immutables.card_type == CardType::LANDMARK;
//}
//[[nodiscard]] inline bool is_fieldcard() const
//{
//   return is_unit() || is_landmark();
//}
//[[nodiscard]] inline bool is_trap() const
//{
//   return m_immutables.card_type == CardType::TRAP;
//}
//[[nodiscard]] inline bool is_skill() const
//{
//   return m_immutables.super_type == CardSuperType::SKILL;
//}
//[[nodiscard]] inline bool is_champion() const
//{
//   return m_immutables.super_type == CardSuperType::CHAMPION;
//}
//[[nodiscard]] inline bool is_follower() const
//{
//   return is_unit() && not is_champion();
//}
//[[nodiscard]] bool is_created() const
//{
//   return utils::has_value(m_immutables.creator_code);
//}
//
//[[nodiscard]] inline bool has_effect(events::EventLabel e_type) const
//{
//   return m_mutables.add_effects.find(e_type) != m_mutables.add_effects.end();
//}
//
//[[nodiscard]] bool has_effect(events::EventLabel e_type, const IEffect& effect) const;
//
//// manipulations
//
//inline void uncover()
//{
//   m_mutables.hidden = false;
//}
//
//void add_effect(events::EventLabel e_type, sptr< IEffect > effect);
//void remove_effect(events::EventLabel e_type, const IEffect& effect);
//
//void store_grant(const sptr< Grant >& grant);
//inline void store_grant(const std::vector< sptr< Grant > >& grants)
//{
//   for(const auto& grant : grants) {
//      store_grant(grant);
//   }
//}
//
///*
// * A defaulted virtual destructor needed bc of inheritance
// */
//~Card() override = default;
//
///*
// *  Basic constructor
// */
//Card(ConstState const_attrs, MutableState var_attrs);
//
///*
// * Copy Constructor
// */
//Card(const Card& card);
//
///*
// * Deleted copy assignment operator
// */
//Card& operator=(const Card&) = delete;
//
///*
// * Move assignment operator
// */
//Card& operator=(Card&&) = delete;
//
///*
// * Move constructor.
// */
//Card(Card&& card) = delete;
//}
//;

#endif  // LORAINE_BASE_H
