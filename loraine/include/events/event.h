
#ifndef LORAINE_EVENT_H
#define LORAINE_EVENT_H

#include <any>
#include <queue>
#include <set>
#include <stack>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

#include "core/gamedefs.h"
#include "core/targeting.h"
#include "event_labels.h"
#include "event_subscriber.h"
#include "utils/types.h"
#include "utils/utils.h"

// forward-declare
class Card;
class FieldCard;
class Unit;
class Spell;
class GameState;

namespace events {

//class IEvent {
//   virtual void trigger(GameState& state) = 0;
//};

template < class Derived, class EventLabelT, typename... Args >
class Event: public utils::CRTP< Event, Derived, EventLabelT, Args... > {
  public:
   Event(Args... args) : m_data{args...} {}

   void trigger(GameState& state) { this->derived()->trigger_impl(state, m_data); };
   constexpr static EventLabel label = EventLabelT::value;

   [[nodiscard]] auto data() { return &m_data; }
   [[nodiscard]] auto data() const { return &m_data; }

  protected:
   using EventData = std::tuple< Args... >;

  private:
   EventData m_data;
};

struct AttackEvent: public Event< AttackEvent, EventLabelType< EventLabel::ATTACK >, Team > {
   void trigger_impl(GameState& state, EventData& data);
};
/*
 * 1st Arg: const sptr<Card>& = Beholder.
 * 1st Arg: const sptr<Card>& = Beholden.
 */
struct BeholdEvent:
    public Event<
       BeholdEvent,
       EventLabelType< EventLabel::BEHOLD >,
       Team,
       const sptr< Card >&,
       const sptr< Card >& > {
   void trigger_impl(GameState& state, EventData& data);
};
struct BlockEvent: public Event< BlockEvent, EventLabelType< EventLabel::BLOCK >, Team > {
   void trigger_impl(GameState& state, EventData& data);
};
/*
 * 1st Arg: const sptr<Card>& = Card which triggered the capture
 * 2nd Arg: const sptr<Unit>& = Captor
 * 3rd Arg: const sptr<Unit>& = Captee
 */
struct CaptureEvent:
    public Event<
       CaptureEvent,
       EventLabelType< EventLabel::CAPTURE >,
       Team,
       const sptr< Card >&,
       const sptr< Unit >&,
       const sptr< Unit >& > {
   void trigger_impl(GameState& state, EventData& data);
};
/*
 * 1st Arg: const sptr<Card>& = The casted floating
 */
struct CastEvent:
    public Event< CastEvent, EventLabelType< EventLabel::CAST >, Team, const sptr< Card >& > {
   void trigger_impl(GameState& state, EventData& data);
};
/*
 * 1st ARg: const sptr<Card>&: The daybreak causing spell
 */
struct DaybreakEvent:
    public Event<
       DaybreakEvent,
       EventLabelType< EventLabel::DAYBREAK >,
       Team,
       const sptr< Card >& > {
   void trigger_impl(GameState& state, EventData& data);
};

/*
 * 1st Arg: const sptr<Card>& = The discard reason
 * 2nd Arg: const sptr<Card>& = The discarded spell
 */
struct DiscardEvent:
    public Event<
       DiscardEvent,
       EventLabelType< EventLabel::DISCARD >,
       Team,
       const sptr< Card >&,
       const sptr< Card >& > {
   void trigger_impl(GameState& state, EventData& data);
};
/*
 * 1st Arg: const sptr<Card>& = The drawn spell
 */
struct DrawCardEvent:
    public Event<
       DrawCardEvent,
       EventLabelType< EventLabel::DRAW_CARD >,
       Team,
       const sptr< Card >& > {
   void trigger_impl(GameState& state, EventData& data);
};

struct EnlightenmentEvent:
    public Event< EnlightenmentEvent, EventLabelType< EventLabel::ENLIGHTENMENT >, Team > {
   void trigger_impl(GameState& state, EventData& data);
};
/*
 * 1st Arg: long = The amount of gems gained
 */
struct GainManagemEvent:
    public Event< GainManagemEvent, EventLabelType< EventLabel::GAIN_MANAGEM >, Team, long > {
   void trigger_impl(GameState& state, EventData& data);
};
/*
 * 1st Arg: const sptr<Card>& = The healing card
 * 2nd Arg: const sptr<Card>& = The healed spell
 * 3rd Arg: long = The amount healed
 */
struct HealUnitEvent:
    public Event<
       HealUnitEvent,
       EventLabelType< EventLabel::HEAL_UNIT >,
       Team,
       const sptr< Card >&,
       const sptr< Card >&,
       long > {
   void trigger_impl(GameState& state, EventData& data);
};
/*
 * 1st Arg: const sptr<Card>& = The leveling champion spell
 */
struct LevelUpEvent:
    public Event<
       LevelUpEvent,
       EventLabelType< EventLabel::LEVEL_UP >,
       Team,
       const sptr< Card >& > {
   void trigger_impl(GameState& state, EventData& data);
};

/*
 * 1st Arg: const sptr<Card>& = Nexus damaging spell
 * 2nd Arg: long = The damage
 */
struct NexusDamageEvent:
    public Event<
       NexusDamageEvent,
       EventLabelType< EventLabel::NEXUS_DAMAGE >,
       Team,
       const sptr< Card >&,
       long > {
   void trigger_impl(GameState& state, EventData& data);
};

/*
 * 1st Arg: const sptr<Card>& = Nexus striking spell
 * 2nd Arg: long = The damage
 */
struct NexusStrikeEvent:
    public Event<
       NexusStrikeEvent,
       EventLabelType< EventLabel::NEXUS_STRIKE >,
       Team,
       const sptr< Card >&,
       long > {
   void trigger_impl(GameState& state, EventData& data);
};

/*
 * 1st Arg: const sptr<Card>& = The nightfall causing spell
 */
struct NightfallEvent:
    public Event<
       NightfallEvent,
       EventLabelType< EventLabel::NIGHTFALL >,
       Team,
       const sptr< Card >& > {
   void trigger_impl(GameState& state, EventData& data);
};
/*
 * 1st Arg: const sptr<Card>& = The played spell
 */
struct PlayEvent:
    public Event< PlayEvent, EventLabelType< EventLabel::PLAY >, Team, const sptr< Card >& > {
   void trigger_impl(GameState& state, EventData& data);
};
/*
 * 1st Arg: const sptr<Card>& = The recalling spell
 * 2nd Arg: const sptr<Card>& = The recalled card
 */
struct RecallEvent:
    public Event<
       RecallEvent,
       EventLabelType< EventLabel::RECALL >,
       Team,
       const sptr< Card >&,
       const sptr< Card >& > {
   void trigger_impl(GameState& state, EventData& data);
};
/*
 * 1st Arg: long = The round number
 */
struct RoundEndEvent:
    public Event< RoundEndEvent, EventLabelType< EventLabel::ROUND_END >, Team, long > {
   void trigger_impl(GameState& state, EventData& data);
};
/*
 * 1st Arg: long = The round number
 */
struct RoundStartEvent:
    public Event< RoundStartEvent, EventLabelType< EventLabel::ROUND_START >, Team, long > {
   void trigger_impl(GameState& state, EventData& data);
};

struct ScoutEvent: public Event< ScoutEvent, EventLabelType< EventLabel::SCOUT >, Team > {
   void trigger_impl(GameState& state, EventData& data);
};
/*
 * 1st Arg: const sptr<Card>& = The killing card
 * 2nd Arg:const sptr<Unit>& = The killed unit
 */
struct SlayEvent:
    public Event<
       SlayEvent,
       EventLabelType< EventLabel::SLAY >,
       Team,
       const sptr< Card >&,
       const sptr< Unit >& > {
   void trigger_impl(GameState& state, EventData& data);
};
/*
 * 1st ARg: const sptr<Unit>& = The striking unit
 * 2nd Arg: const sptr<Unit>& = The struck unit
 */
struct StrikeEvent:
    public Event<
       StrikeEvent,
       EventLabelType< EventLabel::STRIKE >,
       Team,
       const sptr< Unit >&,
       const sptr< Unit >& > {
   void trigger_impl(GameState& state, EventData& data);
};
/*
 * 1st Arg&: const sptr<Card>& = The stunning card
 * 2nd Arg: const sptr<Unit>& = The stunned unit
 */
struct StunEvent:
    public Event<
       StunEvent,
       EventLabelType< EventLabel::STUN >,
       Team,
       const sptr< Card >&,
       const sptr< Unit >& > {
   void trigger_impl(GameState& state, EventData& data);
};
/*
 * 1st Arg: const sptr<FieldCard>& = The summoned fieldcard
 */
struct SummonEvent:
    public Event<
       SummonEvent,
       EventLabelType< EventLabel::SUMMON >,
       Team,
       const sptr< FieldCard >& > {
   void trigger_impl(GameState& state, EventData& data);
};
/*
 * 1st Arg: const sptr<Unit>& = The supporting unit
 * 2nd Arg: const sptr<Unit>& = The supported unit
 */
struct SupportEvent:
    public Event<
       SupportEvent,
       EventLabelType< EventLabel::SUPPORT >,
       Team,
       const sptr< Unit >&,
       const sptr< Unit >& > {
   void trigger_impl(GameState& state, EventData& data);
};
/*
 * 1st Arg: const sptr<Card>& = The targeting card
 * 2nd Arg: const sptr<Unit>& = The targeted unit
 */
struct TargetEvent:
    public Event<
       TargetEvent,
       EventLabelType< EventLabel::TARGET >,
       Team,
       const sptr< Card >&,
       const sptr< Unit >& > {
   void trigger_impl(GameState& state, EventData& data);
};
/*
 * 1st Arg: const sptr<Card>& = The damage causing card
 * 2nd Arg: const sptr<Unit>& = The damaged Unit
 * 3rd Arg: long = The damage amount
 */
struct UnitDamageEvent:
    public Event<
       UnitDamageEvent,
       EventLabelType< EventLabel::UNIT_DAMAGE >,
       Team,
       const sptr< Card >&,
       const sptr< Unit >&,
       long > {
   void trigger_impl(GameState& state, EventData& data);
};

namespace helpers {

template < EventLabel label >
struct label_to_event;

template <>
struct label_to_event< EventLabel::ATTACK > {
   using type = events::AttackEvent;
   using label_type = EventLabelType< EventLabel::ATTACK >;
};
template <>
struct label_to_event< EventLabel::BEHOLD > {
   using type = events::BeholdEvent;
   using label_type = EventLabelType< EventLabel::BEHOLD >;
};
template <>
struct label_to_event< EventLabel::BLOCK > {
   using type = events::BlockEvent;
   using label_type = EventLabelType< EventLabel::BLOCK >;
};
template <>
struct label_to_event< EventLabel::CAPTURE > {
   using type = events::CaptureEvent;
   using label_type = EventLabelType< EventLabel::CAPTURE >;
};
template <>
struct label_to_event< EventLabel::CAST > {
   using type = events::CastEvent;
   using label_type = EventLabelType< EventLabel::CAST >;
};
template <>
struct label_to_event< EventLabel::DAYBREAK > {
   using type = events::DaybreakEvent;
   using label_type = EventLabelType< EventLabel::DAYBREAK >;
};

template <>
struct label_to_event< EventLabel::DISCARD > {
   using type = events::DiscardEvent;
   using label_type = EventLabelType< EventLabel::DISCARD >;
};
template <>
struct label_to_event< EventLabel::DRAW_CARD > {
   using type = events::DrawCardEvent;
   using label_type = EventLabelType< EventLabel::DRAW_CARD >;
};
template <>
struct label_to_event< EventLabel::ENLIGHTENMENT > {
   using type = events::EnlightenmentEvent;
   using label_type = EventLabelType< EventLabel::ENLIGHTENMENT >;
};
template <>
struct label_to_event< EventLabel::GAIN_MANAGEM > {
   using type = events::GainManagemEvent;
   using label_type = EventLabelType< EventLabel::GAIN_MANAGEM >;
};
template <>
struct label_to_event< EventLabel::HEAL_UNIT > {
   using type = events::HealUnitEvent;
   using label_type = EventLabelType< EventLabel::HEAL_UNIT >;
};
template <>
struct label_to_event< EventLabel::LEVEL_UP > {
   using type = events::LevelUpEvent;
   using label_type = EventLabelType< EventLabel::LEVEL_UP >;
};
template <>
struct label_to_event< EventLabel::NEXUS_DAMAGE > {
   using type = events::NexusDamageEvent;
   using label_type = EventLabelType< EventLabel::NEXUS_DAMAGE >;
};
template <>
struct label_to_event< EventLabel::NEXUS_STRIKE > {
   using type = events::NexusStrikeEvent;
   using label_type = EventLabelType< EventLabel::NEXUS_STRIKE >;
};
template <>
struct label_to_event< EventLabel::NIGHTFALL > {
   using type = events::NightfallEvent;
   using label_type = EventLabelType< EventLabel::NIGHTFALL >;
};
template <>
struct label_to_event< EventLabel::PLAY > {
   using type = events::PlayEvent;
   using label_type = EventLabelType< EventLabel::PLAY >;
};
template <>
struct label_to_event< EventLabel::RECALL > {
   using type = events::RecallEvent;
   using label_type = EventLabelType< EventLabel::RECALL >;
};
template <>
struct label_to_event< EventLabel::ROUND_END > {
   using type = events::RoundEndEvent;
   using label_type = EventLabelType< EventLabel::ROUND_END >;
};
template <>
struct label_to_event< EventLabel::ROUND_START > {
   using type = events::RoundStartEvent;
   using label_type = EventLabelType< EventLabel::ROUND_START >;
};
template <>
struct label_to_event< EventLabel::SCOUT > {
   using type = events::ScoutEvent;
   using label_type = EventLabelType< EventLabel::SCOUT >;
};
template <>
struct label_to_event< EventLabel::SLAY > {
   using type = events::SlayEvent;
   using label_type = EventLabelType< EventLabel::SLAY >;
};
template <>
struct label_to_event< EventLabel::STRIKE > {
   using type = events::StrikeEvent;
   using label_type = EventLabelType< EventLabel::STRIKE >;
};
template <>
struct label_to_event< EventLabel::SUMMON > {
   using type = events::SummonEvent;
   using label_type = EventLabelType< EventLabel::SUMMON >;
};
template <>
struct label_to_event< EventLabel::STUN > {
   using type = events::StunEvent;
   using label_type = EventLabelType< EventLabel::STUN >;
};
template <>
struct label_to_event< EventLabel::SUPPORT > {
   using type = events::SupportEvent;
   using label_type = EventLabelType< EventLabel::SUPPORT >;
};
template <>
struct label_to_event< EventLabel::TARGET > {
   using type = events::TargetEvent;
   using label_type = EventLabelType< EventLabel::TARGET >;
};
template <>
struct label_to_event< EventLabel::UNIT_DAMAGE > {
   using type = events::UnitDamageEvent;
   using label_type = EventLabelType< EventLabel::UNIT_DAMAGE >;
};

template < EventLabel elabel >
using label_to_event_t = typename label_to_event< elabel >::type;

template < size_t... Ints >
IEventSubscriber< label_to_event_t< static_cast< EventLabel >(Ints) >... > make_eventsub_interface(
   std::index_sequence< Ints... >);

template < typename... Event >
using SubChannel = std::tuple< std::vector< IEventSubscriber< Event >* >... >;

template < size_t... Ints >
SubChannel< label_to_event_t< static_cast< EventLabel >(Ints) >... > make_subchannel(
   std::index_sequence< Ints... >);

}  // namespace helpers

// using IAllEventSubscriber = std::decay_t< decltype(helpers::make_event_interface(
//    std::declval< std::make_index_sequence< events::n_events > >())) >;

struct IAllEventSubscriber:
    public IEventSubscriber< std::decay_t< decltype(helpers::make_eventsub_interface(
       std::declval< std::make_index_sequence< n_events > >())) > > {
};

using AllSubChannels = std::decay_t< decltype(helpers::make_subchannel(
   std::declval< std::make_index_sequence< n_events > >())) >;

class EventBus;

class LOREvent {
  public:
   using EventVariant = std::variant<
      AttackEvent,
      BeholdEvent,
      BlockEvent,
      CaptureEvent,
      CastEvent,
      DaybreakEvent,
      DiscardEvent,
      DrawCardEvent,
      EnlightenmentEvent,
      GainManagemEvent,
      HealUnitEvent,
      LevelUpEvent,
      NexusDamageEvent,
      NexusStrikeEvent,
      NightfallEvent,
      PlayEvent,
      RecallEvent,
      RoundEndEvent,
      RoundStartEvent,
      ScoutEvent,
      SlayEvent,
      StrikeEvent,
      StunEvent,
      SummonEvent,
      SupportEvent,
      TargetEvent,
      UnitDamageEvent >;

   LOREvent(EventVariant event) noexcept : m_event_detail(std::move(event)) {}

   template < typename DetailType >
   [[nodiscard]] inline auto& detail()
   {
      return std::get< DetailType >(m_event_detail);
   }
   template < typename DetailType >
   [[nodiscard]] inline auto& detail() const
   {
      return std::get< DetailType >(m_event_detail);
   }
   void trigger(EventBus& bus, GameState& state);

   // default for not providing any template type and therefore simply getting the entire variant
   [[nodiscard]] inline auto& detail() const { return m_event_detail; }

   constexpr inline EventLabel label() const
   {
      return std::visit([&](const auto& event) { return event.label; }, m_event_detail);
   }

  private:
   EventVariant m_event_detail;
};

class EventBus {
  private:
   std::queue< LOREvent > m_queue;
   AllSubChannels m_sub_channels;

  protected:
   template < typename Event >
   void _notify(IEventSubscriber< Event >* subscriber, GameState& state, Event& event);

  public:
   template < EventLabel label >
   const auto& subscribers() const
   {
      return std::get< static_cast< size_t >(label) >(m_sub_channels);
   }

   template < EventLabel label, typename... Args >
   void push(Args... args)
   {
      using EventType = helpers::label_to_event< label >;
      constexpr size_t idx = size_t(label);
      std::vector< EventType >& v = std::get< idx > > (m_sub_channels);
      v.emplace_back(LOREvent(EventType{args...}));
   }

   template < EventLabel label >
   void subscribe(IEventSubscriber< helpers::label_to_event_t< label > >* t)
   {
      subscribers< label >().emplace_back(t);
   }

   template < EventLabel label >
   inline void unsubscribe(IEventSubscriber< helpers::label_to_event_t< label > >* sub)
   {
      auto& subs = subscribers< label >();
      subs.erase(std::find(subs.begin(), subs.end(), sub));
   }

   template < typename Event >
   void inform(GameState& state, Event& event)
   {
      for(auto& sub : subscribers< Event::label >()) {
         _notify(sub, state, event);
      }
   }
};

}  // namespace events
#endif  // LORAINE_EVENT_H
