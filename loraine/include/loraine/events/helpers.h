

#ifndef LORAINE_HELPERS_H
#define LORAINE_HELPERS_H

#include "event_id.h"
#include "event.h"

namespace helpers {

using namespace events;

template < EventID label >
struct id_to_event;

template <>
struct id_to_event< EventID::ATTACK > {
   using type = AttackEvent;
   using id = EventIDType< EventID::ATTACK >;
};
template <>
struct id_to_event< EventID::BEHOLD > {
   using type = BeholdEvent;
   using id = EventIDType< EventID::BEHOLD >;
};
template <>
struct id_to_event< EventID::BLOCK > {
   using type = BlockEvent;
   using id = EventIDType< EventID::BLOCK >;
};
template <>
struct id_to_event< EventID::CAPTURE > {
   using type = CaptureEvent;
   using id = EventIDType< EventID::CAPTURE >;
};
template <>
struct id_to_event< EventID::CAST > {
   using type = CastEvent;
   using id = EventIDType< EventID::CAST >;
};
template <>
struct id_to_event< EventID::DAYBREAK > {
   using type = DaybreakEvent;
   using id = EventIDType< EventID::DAYBREAK >;
};

template <>
struct id_to_event< EventID::DISCARD > {
   using type = DiscardEvent;
   using id = EventIDType< EventID::DISCARD >;
};
template <>
struct id_to_event< EventID::DRAW_CARD > {
   using type = DrawCardEvent;
   using id = EventIDType< EventID::DRAW_CARD >;
};
template <>
struct id_to_event< EventID::ENLIGHTENMENT > {
   using type = EnlightenmentEvent;
   using id = EventIDType< EventID::ENLIGHTENMENT >;
};
template <>
struct id_to_event< EventID::GAIN_MANAGEM > {
   using type = GainManagemEvent;
   using id = EventIDType< EventID::GAIN_MANAGEM >;
};
template <>
struct id_to_event< EventID::HEAL_UNIT > {
   using type = HealUnitEvent;
   using id = EventIDType< EventID::HEAL_UNIT >;
};
template <>
struct id_to_event< EventID::LEVEL_UP > {
   using type = LevelUpEvent;
   using id = EventIDType< EventID::LEVEL_UP >;
};
template <>
struct id_to_event< EventID::NEXUS_DAMAGE > {
   using type = NexusDamageEvent;
   using id = EventIDType< EventID::NEXUS_DAMAGE >;
};
template <>
struct id_to_event< EventID::NEXUS_STRIKE > {
   using type = NexusStrikeEvent;
   using id = EventIDType< EventID::NEXUS_STRIKE >;
};
template <>
struct id_to_event< EventID::NIGHTFALL > {
   using type = NightfallEvent;
   using id = EventIDType< EventID::NIGHTFALL >;
};
template <>
struct id_to_event< EventID::PLAY > {
   using type = PlayEvent;
   using id = EventIDType< EventID::PLAY >;
};
template <>
struct id_to_event< EventID::RECALL > {
   using type = RecallEvent;
   using id = EventIDType< EventID::RECALL >;
};
template <>
struct id_to_event< EventID::ROUND_END > {
   using type = RoundEndEvent;
   using id = EventIDType< EventID::ROUND_END >;
};
template <>
struct id_to_event< EventID::ROUND_START > {
   using type = RoundStartEvent;
   using id = EventIDType< EventID::ROUND_START >;
};
template <>
struct id_to_event< EventID::SCOUT > {
   using type = ScoutEvent;
   using id = EventIDType< EventID::SCOUT >;
};
template <>
struct id_to_event< EventID::SLAY > {
   using type = SlayEvent;
   using id = EventIDType< EventID::SLAY >;
};
template <>
struct id_to_event< EventID::STRIKE > {
   using type = StrikeEvent;
   using id = EventIDType< EventID::STRIKE >;
};
template <>
struct id_to_event< EventID::SUMMON > {
   using type = SummonEvent;
   using id = EventIDType< EventID::SUMMON >;
};
template <>
struct id_to_event< EventID::STUN > {
   using type = StunEvent;
   using id = EventIDType< EventID::STUN >;
};
template <>
struct id_to_event< EventID::SUPPORT > {
   using type = SupportEvent;
   using id = EventIDType< EventID::SUPPORT >;
};
template <>
struct id_to_event< EventID::TARGET > {
   using type = TargetEvent;
   using id = EventIDType< EventID::TARGET >;
};
template <>
struct id_to_event< EventID::UNIT_DAMAGE > {
   using type = UnitDamageEvent;
   using id = EventIDType< EventID::UNIT_DAMAGE >;
};

template < EventID elabel >
using id_to_event_t = typename id_to_event< elabel >::type;

template < size_t... Ints >
IEventSubscriber< id_to_event_t< static_cast< EventID >(Ints) >... > make_eventsub_interface(
   std::index_sequence< Ints... >);

template < typename... Events >
using SubChannel = std::tuple< std::vector< IEventSubscriber< Events >* >... >;

template < size_t... Ints >
SubChannel< id_to_event_t< static_cast< EventID >(Ints) >... > make_subchannel(
   std::index_sequence< Ints... >);

template <typename T, typename... Events>
using is_event_of = std::disjunction< std::is_same< T, Events> ... >;
using is_event_of_t = std::disjunction_t< std::is_same< T, Events> ... >;

template <size_t... Ints>
is_event_of<Ints...> make_is_event(std::index_sequence<Ints...>);

}  // namespace helpers

namespace events {

struct is_event =
}

#endif  // LORAINE_HELPERS_H
