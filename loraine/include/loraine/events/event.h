
#ifndef LORAINE_EVENT_H
#define LORAINE_EVENT_H

#include <any>
#include <entt/entt.hpp>
#include <queue>
#include <set>
#include <stack>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

#include "event_id.h"
#include "loraine/utils/utils.h"
#include "loraine/core/gamedefs.h"

class GameState;

namespace events {

template < class event_id_t, typename... Args >
class Event: public utils::CRTP< Event, event_id_t, Args... > {
  public:
   using EventData = std::tuple< GameState&, Args... >;

   Event(Args... args) : m_data{args...} {}
   Event(const Event& event) = default;
   Event(Event&& event) noexcept = default;
   Event& operator=(const Event& event) noexcept = default;
   Event& operator=(Event&& event) noexcept = default;
   ~Event() = default;

   constexpr static EventID label = event_id_t::value;
   [[nodiscard]] auto& state() { return std::get< GameState >(m_data); }
   [[nodiscard]] auto& data() { return m_data; }
   [[nodiscard]] auto& data() const { return m_data; }

  private:
   EventData m_data;
};

using AttackEvent = Event<  EventIDType< EventID::ATTACK >, Team >;
/*
 * 1st Arg: entt::entity = Beholder.
 * 1st Arg: entt::entity = Beholden.
 */

using BeholdEvent =
   Event<  EventIDType< EventID::BEHOLD >, Team, entt::entity, entt::entity >;


using BlockEvent = Event<  EventIDType< EventID::BLOCK >, Team >;
/*
 * 1st Arg: entt::entity = Card which triggered the capture
 * 2nd Arg: entt::entity = Captor
 * 3rd Arg: entt::entity = Captee
 */

using CaptureEvent = Event<

   EventIDType< EventID::CAPTURE >,
   Team,
   entt::entity,
   entt::entity,
   entt::entity >;
/*
 * 1st Arg: entt::entity = The casted floating
 */

using CastEvent = Event<  EventIDType< EventID::CAST >, Team, entt::entity >;
/*
 * 1st ARg: entt::entity: The daybreak causing spell
 */

using DaybreakEvent =
   Event<  EventIDType< EventID::DAYBREAK >, Team, entt::entity >;

/*
 * 1st Arg: entt::entity = The discard reason
 * 2nd Arg: entt::entity = The discarded spell
 */

using DiscardEvent =
   Event<  EventIDType< EventID::DISCARD >, Team, entt::entity, entt::entity >;
/*
 * 1st Arg: entt::entity = The drawn spell
 */

using DrawCardEvent =
   Event<  EventIDType< EventID::DRAW_CARD >, Team, entt::entity >;


using EnlightenmentEvent = Event<

   EventIDType< EventID::ENLIGHTENMENT >,
   Team >;
/*
 * 1st Arg: long = The amount of gems gained
 */

using GainManagemEvent =
   Event<  EventIDType< EventID::GAIN_MANAGEM >, Team, long >;
/*
 * 1st Arg: entt::entity = The healing card
 * 2nd Arg: entt::entity = The healed spell
 * 3rd Arg: long = The amount healed
 */

using HealUnitEvent = Event<

   EventIDType< EventID::HEAL_UNIT >,
   Team,
   entt::entity,
   entt::entity,
   long >;
/*
 * 1st Arg: entt::entity = The leveling champion spell
 */

using LevelUpEvent =
   Event<  EventIDType< EventID::LEVEL_UP >, Team, entt::entity >;

/*
 * 1st Arg: entt::entity = Nexus damaging spell
 * 2nd Arg: long = The damage
 */

using NexusDamageEvent =
   Event<  EventIDType< EventID::NEXUS_DAMAGE >, Team, entt::entity, long >;

/*
 * 1st Arg: entt::entity = Nexus striking spell
 * 2nd Arg: long = The damage
 */

using NexusStrikeEvent =
   Event<  EventIDType< EventID::NEXUS_STRIKE >, Team, entt::entity, long >;

/*
 * 1st Arg: entt::entity = The nightfall causing spell
 */

using NightfallEvent =
   Event<  EventIDType< EventID::NIGHTFALL >, Team, entt::entity >;
/*
 * 1st Arg: entt::entity = The played spell
 */
using PlayEvent = Event<  EventIDType< EventID::PLAY >, Team, entt::entity >;

/*
 * 1st Arg: entt::entity = The recalling card
 * 2nd Arg: entt::entity = The recalled card
 */
using RecallEvent =
Event<  EventIDType< EventID::RECALL >, Team, entt::entity, entt::entity >;

/*
 * 1st Arg: entt::entity = The striking unit
 */
using ReputationEvent =
Event<  EventIDType< EventID::REPUTATION >, Team, entt::entity>;

/*
 * 1st Arg: long = The round number
 */

using RoundEndEvent = Event<  EventIDType< EventID::ROUND_END >, Team, long >;
/*
 * 1st Arg: long = The round number
 */

using RoundStartEvent =
   Event<  EventIDType< EventID::ROUND_START >, Team, long >;


using ScoutEvent = Event<  EventIDType< EventID::SCOUT >, Team >;
/*
 * 1st Arg: entt::entity = The killing card
 * 2nd Arg:entt::entity = The killed unit
 */

using SlayEvent =
   Event<  EventIDType< EventID::SLAY >, Team, entt::entity, entt::entity >;
/*
 * 1st Arg: entt::entity = The striking unit
 * 2nd Arg: entt::entity = The struck unit
 */

using StrikeEvent =
   Event<  EventIDType< EventID::STRIKE >, Team, entt::entity, entt::entity >;
/*
 * 1st Arg&: entt::entity = The stunning card
 * 2nd Arg: entt::entity = The stunned unit
 */

using StunEvent =
   Event<  EventIDType< EventID::STUN >, Team, entt::entity, entt::entity >;
/*
 * 1st Arg: entt::entity = The summoned fieldcard
 */

using SummonEvent =
   Event<  EventIDType< EventID::SUMMON >, Team, entt::entity >;
/*
 * 1st Arg: entt::entity = The supporting unit
 * 2nd Arg: entt::entity = The supported unit
 */

using SupportEvent =
   Event<  EventIDType< EventID::SUPPORT >, Team, entt::entity, entt::entity >;
/*
 * 1st Arg: entt::entity = The targeting card
 * 2nd Arg: entt::entity = The targeted unit
 */

using TargetEvent =
   Event<  EventIDType< EventID::TARGET >, Team, entt::entity, entt::entity >;
/*
 * 1st Arg: entt::entity = The damage causing card
 * 2nd Arg: entt::entity = The damaged Unit
 * 3rd Arg: long = The damage amount
 */

using UnitDamageEvent = Event<

   EventIDType< EventID::UNIT_DAMAGE >,
   Team,
   entt::entity,
   entt::entity,
   long >;

// using IAllEventSubscriber = std::decay_t< decltype(helpers::make_event_interface(
//    std::declval< std::make_index_sequence< events::n_events > >())) >;

// class EventBus;

// class LOREvent {
//   public:
//    using EventVariant = std::variant<
//       AttackEvent,
//       BeholdEvent,
//       BlockEvent,
//       CaptureEvent,
//       CastEvent,
//       DaybreakEvent,
//       DiscardEvent,
//       DrawCardEvent,
//       EnlightenmentEvent,
//       GainManagemEvent,
//       HealUnitEvent,
//       LevelUpEvent,
//       NexusDamageEvent,
//       NexusStrikeEvent,
//       NightfallEvent,
//       PlayEvent,
//       RecallEvent,
//       RoundEndEvent,
//       RoundStartEvent,
//       ScoutEvent,
//       SlayEvent,
//       StrikeEvent,
//       StunEvent,
//       SummonEvent,
//       SupportEvent,
//       TargetEvent,
//       UnitDamageEvent >;
//
//    LOREvent(EventVariant event) noexcept : m_event_detail(std::move(event)) {}
//
//    template < typename DetailType >
//    [[nodiscard]] inline auto& detail()
//    {
//       return std::get< DetailType >(m_event_detail);
//    }
//    template < typename DetailType >
//    [[nodiscard]] inline auto& detail() const
//    {
//       return std::get< DetailType >(m_event_detail);
//    }
//    void trigger(EventBus& bus, GameState& state);
//
//    // default for not providing any template type and therefore simply getting the entire variant
//    [[nodiscard]] inline auto& detail() const { return m_event_detail; }
//
//    constexpr inline EventID label() const
//    {
//       return std::visit([&](const auto& event) { return event.label; }, m_event_detail);
//    }
//
//   private:
//    EventVariant m_event_detail;
// };

}  // namespace events
#endif  // LORAINE_EVENT_H
