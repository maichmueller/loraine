
#ifndef LORAINE_EVENT_H
#define LORAINE_EVENT_H

#include <stack>
#include <utility>
#include <variant>
#include <vector>

#include "event_types.h"
#include "rulesets.h"
#include "target.h"
#include "types.h"

// forward-declare Card, Unit, Spell
class Card;
class Unit;
class Spell;

namespace events {

class AnyEvent {
   const EventType m_event_type;
   const Player m_causing_player;
   const std::optional< const std::vector< sptr< Card > > > m_causing_cards;
   const std::optional< const std::vector< sptr< BaseTarget > > > m_targets;
   const std::optional< const std::vector< sptr< long > > > m_values;

  public:
   virtual ~AnyEvent() = default;
   AnyEvent(
      EventType type,
      Player causing_player,
      std::vector< sptr< Card > > causing_cards = {},
      std::vector< sptr< BaseTarget > > targets = {},
      std::vector< sptr< long > > values = {})
       : m_event_type(type),
         m_causing_player(causing_player),
         m_causing_cards(causing_cards),
         m_targets(targets),
         m_values(values)
   {
   }
   [[nodiscard]] inline auto get_event_type() const { return m_event_type; }
   [[nodiscard]] inline auto get_player() const { return m_causing_player; }
   [[nodiscard]] inline auto get_causing_cards() const { return m_causing_cards; }
   [[nodiscard]] inline auto get_targets() const { return m_targets; }
   [[nodiscard]] inline auto get_values() const { return m_values; }
};

class NoneEvent: public AnyEvent {
  public:
   NoneEvent() : AnyEvent(EventType::NONE, BLUE) {}
};
class AttackEvent: public AnyEvent {
  public:
   explicit AttackEvent(Player player) : AnyEvent(EventType::ATTACK, player) {}
};
class BeholdEvent: public AnyEvent {
  public:
   explicit BeholdEvent(Player player) : AnyEvent(EventType::BEHOLD, player) {}
};
class BlockEvent: public AnyEvent {
  public:
   explicit BlockEvent(Player player) : AnyEvent(EventType::BLOCK, player) {}
};
class CaptureEvent: public AnyEvent {
  public:
   static const EventType event_type = EventType::CAPTURE;
   CaptureEvent(Player player, const sptr< Card >& causing_card, sptr< BaseTarget > captor, sptr< BaseTarget > captee)
       : AnyEvent(
          EventType::CAPTURE, player, {causing_card}, {std::move(captor), std::move(captee)})
   {
   }
};
class CastEvent: public AnyEvent {
  public:
   CastEvent(Player player, sptr< Card > casted_spell)
       : AnyEvent(EventType::CAST, player, {std::move(casted_spell)})
   {
   }
};
class DaybreakEvent: public AnyEvent {
  public:
   static const EventType event_type = EventType::DAYBREAK;
   DaybreakEvent(Player player, sptr< Card > causing_card)
       : AnyEvent(EventType::DAYBREAK, player, {std::move(causing_card)})
   {
   }
};
class DieEvent: public AnyEvent {
  public:
   static const EventType event_type = EventType::DIE;
   DieEvent(Player player, sptr< BaseTarget > killed, sptr< Card > killer)
       : AnyEvent(EventType::DIE, player, {std::move(killer)}, {std::move(killed)})
   {
   }
   DieEvent(Player player, const std::vector<sptr< BaseTarget > >& killed_units, sptr< Card > killer)
       : AnyEvent(EventType::DIE, player, {std::move(killer)}, killed_units)
   {
   }
};
class DiscardEvent: public AnyEvent {
  public:
   DiscardEvent(Player player, sptr< Card > causing_card, sptr< BaseTarget > discarded_card)
       : AnyEvent(
          EventType::DISCARD, player, {std::move(causing_card)}, {std::move(discarded_card)})
   {
   }
};
class DrawCardEvent: public AnyEvent {
  public:
   DrawCardEvent(Player player, sptr< Card > card)
       : AnyEvent(EventType::DRAW_CARD, player, {std::move(card)})
   {
   }
};
class EnlightenmentEvent: public AnyEvent {
  public:
   explicit EnlightenmentEvent(Player player) : AnyEvent(EventType::ENLIGHTENMENT, player) {}
};
class GainManagemEvent: public AnyEvent {
  public:
   static const EventType event_type = EventType::GAIN_MANAGEM;
   GainManagemEvent(Player player, sptr< long > amount)
       : AnyEvent(EventType::GAIN_MANAGEM, player, {}, {}, {std::move(amount)})
   {
   }
};
class HealUnitEvent: public AnyEvent {
  public:
   HealUnitEvent(Player player, sptr< Card > healed_unit, sptr< long > amount)
       : AnyEvent(
          EventType::HEAL_UNIT,
          player,
          {},
          {std::make_shared<CardTarget>(player, Location::BOARD, 0, std::move(healed_unit))},
          {std::move(amount)})
   {
   }
};
class LevelUpEvent: public AnyEvent {
  public:
   LevelUpEvent(Player player, sptr< Card > leveling_champ)
       : AnyEvent(EventType::LEVEL_UP, player, {std::move(leveling_champ)})
   {
   }
};
class NexusStrikeEvent: public AnyEvent {
  public:
   NexusStrikeEvent(
      Player attacking_player,
      sptr< Card > attacking_card,
      Player attacked_nexus,
      sptr< long > damage)
       : AnyEvent(
          EventType::NEXUS_STRIKE,
          attacking_player,
          {std::move(attacking_card)},
          {std::make_shared<NexusTarget>(attacked_nexus)},
          {std::move(damage)})
   {
   }
};
class NightfallEvent: public AnyEvent {
  public:
   NightfallEvent(Player player, sptr< Card > card) : AnyEvent(EventType::NIGHTFALL, player, {card})
   {
   }
};
class PlayEvent: public AnyEvent {
  public:
   PlayEvent(Player player, sptr< Card > card)
       : AnyEvent(EventType::PLAY, player, {std::move(card)})
   {
   }
};
class RecallEvent: public AnyEvent {
  public:
   RecallEvent(Player player, sptr< Card > card)
       : AnyEvent(EventType::RECALL, player, {std::move(card)})
   {
   }
};
class RoundEndEvent: public AnyEvent {
  public:
   RoundEndEvent(Player player, sptr< long > round)
       : AnyEvent(EventType::ROUND_END, player, {}, {}, {std::move(round)})
   {
   }
};
class RoundStartEvent: public AnyEvent {
  public:
   RoundStartEvent(Player player, sptr< long > round)
       : AnyEvent(EventType::ROUND_START, player, {}, {}, {std::move(round)})
   {
   }
};
class ScoutEvent: public AnyEvent {
  public:
   explicit ScoutEvent(Player player) : AnyEvent(EventType::SCOUT, player) {}
};
class StrikeEvent: public AnyEvent {
  public:
   StrikeEvent(Player player, sptr< Card > striker)
       : AnyEvent(EventType::STRIKE, player, {std::move(striker)})
   {
   }
};
class SummonEvent: public AnyEvent {
  public:
   SummonEvent(Player player, sptr< Card > card) : AnyEvent(EventType::SUMMON, player, {card}) {}
};
class StunEvent: public AnyEvent {
  public:
   StunEvent(Player player, sptr< Card > card)
       : AnyEvent(EventType::STUN, player, {std::move(card)})
   {
   }
};
class TargetEvent: public AnyEvent {
  public:
   TargetEvent(Player player, sptr< Card > cause, sptr< BaseTarget > target)
       : AnyEvent(EventType::TARGET, player, {std::move(cause)}, {std::move(target)})
   {
   }
   template < typename Container >
   TargetEvent(Player player, sptr< Card > cause, Container targets)
       : AnyEvent(EventType::TARGET, player, {std::move(cause)}, {std::move(targets)})
   {
   }
};
class UnitTakeDamageEvent: public AnyEvent {
  public:
   UnitTakeDamageEvent(Player player, sptr< Card > cause, sptr< BaseTarget > unit, sptr< long > damage)
       : AnyEvent(
          EventType::UNIT_TAKE_DAMAGE,
          player,
          {std::move(cause)},
          {std::move(unit)},
          {std::move(damage)})
   {
   }
};

//// The most recent happening event
// using VariantEvent = std::variant<
//   AnyEvent,
//   NoneEvent,
//   //   BattleEvent,
//   AttackEvent,
//   BeholdEvent,
//   CaptureEvent,
//   CastEvent,
//   BlockEvent,
//   DaybreakEvent,
//   DieEvent,
//   DiscardEvent,
//   DrawCardEvent,
//   EndRoundEvent,
//   EnlightenmentEvent,
//   GainManaEvent,
//   LevelUpEvent,
//   NexusStrikeEvent,
//   NightfallEvent,
//   PlayEvent,
//   RecallEvent,
//   RoundEndEvent,
//   RoundStartEvent,
//   StrikeEvent,
//   StunEvent,
//   SummonEvent,
//   TargetEvent,
//   UnitTakeDamageEvent >;
//
//// visitors to access the data depending on the event
// struct VisitorEventType {
//   template < typename event_t >
//   EventType operator()(const event_t& eve) const
//   {
//      return eve.event_type;
//   }
//};

// inline EventType get_event_type(const VariantEvent& event)
//{
//   return std::visit(VisitorEventType{}, event);
//}

// template< typename Event>
// inline auto get_event_data(const AnyEvent& event)
//{
//   return event.get_event_data();
//}

}  // namespace events

#endif  // LORAINE_EVENT_H
