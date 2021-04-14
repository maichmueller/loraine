
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

namespace events {

// class IEvent {
//    virtual void trigger(GameState& state) = 0;
// };

template < class Derived, class EventLabelT, typename... Args >
class Event: public utils::CRTP< Event, Derived, EventLabelT, Args... > {
  public:
   Event(Args... args) : m_data{args...} {}

   template < typename GameStateType >
   void trigger(GameStateType& state)
   {
      this->derived()->trigger_impl(state, m_data);
   };
   constexpr static EventLabel label = EventLabelT::value;

   [[nodiscard]] auto data() { return &m_data; }
   [[nodiscard]] auto data() const { return &m_data; }

  protected:
   using EventData = std::tuple< Args... >;

  private:
   EventData m_data;
};

struct AttackEvent: public Event< AttackEvent, EventLabelType< EventLabel::ATTACK >, Team > {
   template < typename GameStateType >
   void trigger_impl(GameStateType& state, EventData& data);
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
   template < typename GameStateType >
   void trigger_impl(GameStateType& state, EventData& data);
};
struct BlockEvent: public Event< BlockEvent, EventLabelType< EventLabel::BLOCK >, Team > {
   template < typename GameStateType >
   void trigger_impl(GameStateType& state, EventData& data);
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
   template < typename GameStateType >
   void trigger_impl(GameStateType& state, EventData& data);
};
/*
 * 1st Arg: const sptr<Card>& = The casted floating
 */
struct CastEvent:
    public Event< CastEvent, EventLabelType< EventLabel::CAST >, Team, const sptr< Card >& > {
   template < typename GameStateType >
   void trigger_impl(GameStateType& state, EventData& data);
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
   template < typename GameStateType >
   void trigger_impl(GameStateType& state, EventData& data);
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
   template < typename GameStateType >
   void trigger_impl(GameStateType& state, EventData& data);
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
   template < typename GameStateType >
   void trigger_impl(GameStateType& state, EventData& data);
};

struct EnlightenmentEvent:
    public Event< EnlightenmentEvent, EventLabelType< EventLabel::ENLIGHTENMENT >, Team > {
   template < typename GameStateType >
   void trigger_impl(GameStateType& state, EventData& data);
};
/*
 * 1st Arg: long = The amount of gems gained
 */
struct GainManagemEvent:
    public Event< GainManagemEvent, EventLabelType< EventLabel::GAIN_MANAGEM >, Team, long > {
   template < typename GameStateType >
   void trigger_impl(GameStateType& state, EventData& data);
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
   template < typename GameStateType >
   void trigger_impl(GameStateType& state, EventData& data);
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
   template < typename GameStateType >
   void trigger_impl(GameStateType& state, EventData& data);
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
   template < typename GameStateType >
   void trigger_impl(GameStateType& state, EventData& data);
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
   template < typename GameStateType >
   void trigger_impl(GameStateType& state, EventData& data);
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
   template < typename GameStateType >
   void trigger_impl(GameStateType& state, EventData& data);
};
/*
 * 1st Arg: const sptr<Card>& = The played spell
 */
struct PlayEvent:
    public Event< PlayEvent, EventLabelType< EventLabel::PLAY >, Team, const sptr< Card >& > {
   template < typename GameStateType >
   void trigger_impl(GameStateType& state, EventData& data);
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
   template < typename GameStateType >
   void trigger_impl(GameStateType& state, EventData& data);
};
/*
 * 1st Arg: long = The round number
 */
struct RoundEndEvent:
    public Event< RoundEndEvent, EventLabelType< EventLabel::ROUND_END >, Team, long > {
   template < typename GameStateType >
   void trigger_impl(GameStateType& state, EventData& data);
};
/*
 * 1st Arg: long = The round number
 */
struct RoundStartEvent:
    public Event< RoundStartEvent, EventLabelType< EventLabel::ROUND_START >, Team, long > {
   template < typename GameStateType >
   void trigger_impl(GameStateType& state, EventData& data);
};

struct ScoutEvent: public Event< ScoutEvent, EventLabelType< EventLabel::SCOUT >, Team > {
   template < typename GameStateType >
   void trigger_impl(GameStateType& state, EventData& data);
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
   template < typename GameStateType >
   void trigger_impl(GameStateType& state, EventData& data);
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
   template < typename GameStateType >
   void trigger_impl(GameStateType& state, EventData& data);
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
   template < typename GameStateType >
   void trigger_impl(GameStateType& state, EventData& data);
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
   template < typename GameStateType >
   void trigger_impl(GameStateType& state, EventData& data);
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
   template < typename GameStateType >
   void trigger_impl(GameStateType& state, EventData& data);
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
   template < typename GameStateType >
   void trigger_impl(GameStateType& state, EventData& data);
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
   template < typename GameStateType >
   void trigger_impl(GameStateType& state, EventData& data);
};

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
//    constexpr inline EventLabel label() const
//    {
//       return std::visit([&](const auto& event) { return event.label; }, m_event_detail);
//    }
//
//   private:
//    EventVariant m_event_detail;
// };

}  // namespace events
#endif  // LORAINE_EVENT_H
