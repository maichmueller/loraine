

#ifndef LORAINE_HELPERS_H
#define LORAINE_HELPERS_H

#include "event_labels.h"
#include "event.h"

namespace helpers {

using namespace events;

template < EventLabel label >
struct label_to_event;

template <>
struct label_to_event< EventLabel::ATTACK > {
   using type = AttackEvent;
   using label_type = EventLabelType< EventLabel::ATTACK >;
};
template <>
struct label_to_event< EventLabel::BEHOLD > {
   using type = BeholdEvent;
   using label_type = EventLabelType< EventLabel::BEHOLD >;
};
template <>
struct label_to_event< EventLabel::BLOCK > {
   using type = BlockEvent;
   using label_type = EventLabelType< EventLabel::BLOCK >;
};
template <>
struct label_to_event< EventLabel::CAPTURE > {
   using type = CaptureEvent;
   using label_type = EventLabelType< EventLabel::CAPTURE >;
};
template <>
struct label_to_event< EventLabel::CAST > {
   using type = CastEvent;
   using label_type = EventLabelType< EventLabel::CAST >;
};
template <>
struct label_to_event< EventLabel::DAYBREAK > {
   using type = DaybreakEvent;
   using label_type = EventLabelType< EventLabel::DAYBREAK >;
};

template <>
struct label_to_event< EventLabel::DISCARD > {
   using type = DiscardEvent;
   using label_type = EventLabelType< EventLabel::DISCARD >;
};
template <>
struct label_to_event< EventLabel::DRAW_CARD > {
   using type = DrawCardEvent;
   using label_type = EventLabelType< EventLabel::DRAW_CARD >;
};
template <>
struct label_to_event< EventLabel::ENLIGHTENMENT > {
   using type = EnlightenmentEvent;
   using label_type = EventLabelType< EventLabel::ENLIGHTENMENT >;
};
template <>
struct label_to_event< EventLabel::GAIN_MANAGEM > {
   using type = GainManagemEvent;
   using label_type = EventLabelType< EventLabel::GAIN_MANAGEM >;
};
template <>
struct label_to_event< EventLabel::HEAL_UNIT > {
   using type = HealUnitEvent;
   using label_type = EventLabelType< EventLabel::HEAL_UNIT >;
};
template <>
struct label_to_event< EventLabel::LEVEL_UP > {
   using type = LevelUpEvent;
   using label_type = EventLabelType< EventLabel::LEVEL_UP >;
};
template <>
struct label_to_event< EventLabel::NEXUS_DAMAGE > {
   using type = NexusDamageEvent;
   using label_type = EventLabelType< EventLabel::NEXUS_DAMAGE >;
};
template <>
struct label_to_event< EventLabel::NEXUS_STRIKE > {
   using type = NexusStrikeEvent;
   using label_type = EventLabelType< EventLabel::NEXUS_STRIKE >;
};
template <>
struct label_to_event< EventLabel::NIGHTFALL > {
   using type = NightfallEvent;
   using label_type = EventLabelType< EventLabel::NIGHTFALL >;
};
template <>
struct label_to_event< EventLabel::PLAY > {
   using type = PlayEvent;
   using label_type = EventLabelType< EventLabel::PLAY >;
};
template <>
struct label_to_event< EventLabel::RECALL > {
   using type = RecallEvent;
   using label_type = EventLabelType< EventLabel::RECALL >;
};
template <>
struct label_to_event< EventLabel::ROUND_END > {
   using type = RoundEndEvent;
   using label_type = EventLabelType< EventLabel::ROUND_END >;
};
template <>
struct label_to_event< EventLabel::ROUND_START > {
   using type = RoundStartEvent;
   using label_type = EventLabelType< EventLabel::ROUND_START >;
};
template <>
struct label_to_event< EventLabel::SCOUT > {
   using type = ScoutEvent;
   using label_type = EventLabelType< EventLabel::SCOUT >;
};
template <>
struct label_to_event< EventLabel::SLAY > {
   using type = SlayEvent;
   using label_type = EventLabelType< EventLabel::SLAY >;
};
template <>
struct label_to_event< EventLabel::STRIKE > {
   using type = StrikeEvent;
   using label_type = EventLabelType< EventLabel::STRIKE >;
};
template <>
struct label_to_event< EventLabel::SUMMON > {
   using type = SummonEvent;
   using label_type = EventLabelType< EventLabel::SUMMON >;
};
template <>
struct label_to_event< EventLabel::STUN > {
   using type = StunEvent;
   using label_type = EventLabelType< EventLabel::STUN >;
};
template <>
struct label_to_event< EventLabel::SUPPORT > {
   using type = SupportEvent;
   using label_type = EventLabelType< EventLabel::SUPPORT >;
};
template <>
struct label_to_event< EventLabel::TARGET > {
   using type = TargetEvent;
   using label_type = EventLabelType< EventLabel::TARGET >;
};
template <>
struct label_to_event< EventLabel::UNIT_DAMAGE > {
   using type = UnitDamageEvent;
   using label_type = EventLabelType< EventLabel::UNIT_DAMAGE >;
};

template < EventLabel elabel >
using label_to_event_t = typename label_to_event< elabel >::type;

template < size_t... Ints >
IEventSubscriber< label_to_event_t< static_cast< EventLabel >(Ints) >... > make_eventsub_interface(
   std::index_sequence< Ints... >);

template < typename... Events >
using SubChannel = std::tuple< std::vector< IEventSubscriber< Events >* >... >;

template < size_t... Ints >
SubChannel< label_to_event_t< static_cast< EventLabel >(Ints) >... > make_subchannel(
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
