
#ifndef LORAINE_EVENT_H
#define LORAINE_EVENT_H

#include <stack>
#include <utility>
#include <variant>
#include <vector>

#include "rulesets.h"
#include "types.h"

// forward-declare Card, Unit, Spell
class Card;
class Unit;
class Spell;

namespace events {

enum class EventType {
   NONE = 0,
   ATTACK,  // declaring attacker
   //   BATTLE,  // battle commences
   BEHOLD,  // the player has an 8+ mana unit in hand or in play
   CAST,  // casting spells/skills (activation event, not committing event)
   BLOCK,  // declaring blockers
   DAYBREAK,  // the played card is the first to be played in this round
   DIE,  // a unit dies
   DISCARD,  // removing a card from hand (not playing)
   DRAW_CARD,  // draw a card (own deck or enemy deck)
   END_ROUND,  // the round ends
   ENLIGHTENMENT,  // reaching 10 mana gems
   GAIN_MANAGEM,  // gaining an amount of mana (e.g. round-start)
   GET_CARD,  // get a cards from
   LEVEL_UP,  // champion levels up
   NEXUS_STRIKE,  // from direct attack
   NIGHTFALL,  // the played card is not the first to be played in this round
   PLAY_UNIT,  // summoning a unit from hand
   RECALL,  // a unit gets recalled from field back to the hand
   ROUND_END,  // the round ends
   ROUND_START,  // the round starts
   STRIKE,  // a unit strikes (anything)
   STUN,  // stunning a unit
   SUMMON,  // summoning a unit
   TARGET,  // targeting a unit with a spell/skill (
            // 'targeting' effects only occur, when the unit is actually hit)
            // 'being targeted' effects only occur, when the unit survives)
   UNIT_TAKE_DAMAGE,  // any unit on the field takes damage
   USE_MANA,  // using mana (play units, cast spells etc.)
};

struct AnyEvent {
   static const EventType event_type = EventType::NONE;
   /*
    * The base class method to return all base member variables
    */
   [[nodiscard]] static auto get_event_data() { return std::tuple{event_type}; }

  protected:
   /*
    * This is a protected stack of methods to allow the child classes to simply
    * call _get_event_data() in their own public get_event_data() methods
    * without needing to constantly copy over arguments from the base class into
    * the method call. This will be done automatically by the methods below!
    */
   template < typename Tuple, size_t... I, typename... Args >
   [[nodiscard]] auto _get_event_data_base(
      Tuple t, std::index_sequence< I... > /*unused*/, Args... args) const
   {
      return std::tuple{std::get< I >(t)..., args...};
   }
   template < typename Tuple, typename... Args >
   [[nodiscard]] auto _get_event_data_base_applied(Tuple t, Args... args) const
   {
      static constexpr auto size = std::tuple_size< Tuple >::value;
      return _get_event_data_base(
         t, std::make_index_sequence< size >{}, args...);
   }
   // This is now the protected method to call when overriding the child's
   // method get_event_data() in the form of
   // auto get_event_data() {
   //    return _get_event_data(child_member_variable1,
   //       child_member_variable2, ...);
   // }
   template < typename... Args >
   [[nodiscard]] auto _get_event_data(Args... args) const
   {
      return _get_event_data_base_applied(get_event_data(), args...);
   }

  public:
   virtual ~AnyEvent() = default;
   AnyEvent() = default;
};

class AttackEvent: public AnyEvent {
   PLAYER m_player;
   std::vector< size_t > m_positions;

  public:
   static const EventType event_type = EventType::ATTACK;
   AttackEvent(PLAYER player, std::vector< size_t > positions)
       : m_player(player), m_positions(std::move(positions))
   {
   }
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player, m_positions);
   }
};
// struct BattleEvent: public AnyEvent {
//   static const EventType event_type = EventType::BATTLE;
//   explicit BattleEvent() : AnyEvent() {}
//};
class BeholdEvent: public AnyEvent {
   PLAYER m_player;

  public:
   static const EventType event_type = EventType::BEHOLD;
   explicit BeholdEvent(PLAYER player) : m_player(player) {}

   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player);
   }
};
class BlockEvent: public AnyEvent {
   PLAYER m_player;
   std::vector< size_t > m_positions;

  public:
   static const EventType event_type = EventType::BLOCK;
   BlockEvent(PLAYER player, std::vector< size_t > positions)
       : m_player(player), m_positions(std::move(positions))
   {
   }
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player, m_positions);
   }
};
class CastEvent: public AnyEvent {
   PLAYER m_player;
   sptr< Spell > m_spell;
   bool m_is_spell;

  public:
   static const EventType event_type = EventType::CAST;
   CastEvent(PLAYER player, sptr< Spell > spell, bool is_spell)
      : m_player(player), m_spell(std::move(spell)), m_is_spell(is_spell)
   {
   }
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player, m_spell, m_is_spell);
   }
};
class DaybreakEvent: public AnyEvent {
   PLAYER m_player;
   sptr< Card > m_card;

  public:
   static const EventType event_type = EventType::DAYBREAK;
   DaybreakEvent(PLAYER player, sptr< Card > card)
       : m_player(player), m_card(std::move(card))
   {
   }
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player, m_card);
   }
};
class DieEvent: public AnyEvent {
   PLAYER m_player;
   sptr< Unit > m_killed;
   sptr< Card > m_killer;

  public:
   static const EventType event_type = EventType::DIE;
   DieEvent(PLAYER player, sptr< Unit > killed, sptr< Card > killer)
       : m_player(player),
         m_killed(std::move(killed)),
         m_killer(std::move(killer))
   {
   }
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player, m_killed, m_killer);
   }
};
class DiscardEvent: public AnyEvent {
   PLAYER m_player;
   CardID m_card_id;
   sptr< Card > m_card;

  public:
   static const EventType event_type = EventType::DISCARD;
   DiscardEvent(PLAYER player, CardID card_id, sptr< Card > card)
       : m_player(player), m_card_id(card_id), m_card(std::move(card))
   {
   }
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player, m_card_id, m_card);
   }
};
class DrawCardEvent: public AnyEvent {
   PLAYER m_player;
   sptr< Card > m_card;

  public:
   static const EventType event_type = EventType::DRAW_CARD;
   DrawCardEvent(PLAYER player, sptr< Card > card)
       : m_player(player), m_card(std::move(card))
   {
   }
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player, m_card);
   }
};
struct EndRoundEvent: public AnyEvent {
   static const EventType event_type = EventType::END_ROUND;
};
class EnlightenmentEvent: public AnyEvent {
   PLAYER m_player;

  public:
   static const EventType event_type = EventType::ENLIGHTENMENT;
   EnlightenmentEvent(PLAYER player) : m_player(player) {}
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player);
   }
};
class GainManaEvent: public AnyEvent {
   PLAYER m_player;
   size_t m_amount;

  public:
   static const EventType event_type = EventType::GAIN_MANAGEM;
   GainManaEvent(PLAYER player, size_t amount)
       : m_player(player), m_amount(amount)
   {
   }
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player, m_amount);
   }
};
class GetCardEvent: public AnyEvent {
   PLAYER m_player;
   CardID m_card_id;

  public:
   static const EventType event_type = EventType::GET_CARD;
   GetCardEvent(PLAYER player, CardID card_id)
       : m_player(player), m_card_id(card_id)
   {
   }
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player, m_card_id);
   }
};
class LevelUpEvent: public AnyEvent {
   PLAYER m_player;
   sptr< Card > m_card;

  public:
   static const EventType event_type = EventType::LEVEL_UP;
   LevelUpEvent(PLAYER player, sptr< Card > card)
       : m_player(player), m_card(std::move(card))
   {
   }
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player, m_card);
   }
};
class NexusStrikeEvent: public AnyEvent {
   PLAYER m_attacked_nexus;
   PLAYER m_attacking_player;
   sptr< Card > m_attacking_card;
   sptr< size_t > m_damage;
   bool m_direct_strike;  // whether the attack was from hitting the nexus
                          // TRUE: in battle
                          // FALSE: through effects/spells

  public:
   static const EventType event_type = EventType::NEXUS_STRIKE;
   NexusStrikeEvent(
      PLAYER attacked_nexus,
      PLAYER attacking_player,
      sptr< size_t > damage,
      sptr< Card > attacking_card,
      bool direct_strike)
       : m_attacked_nexus(attacked_nexus),
         m_attacking_player(attacking_player),
         m_attacking_card(std::move(attacking_card)),
         m_damage(std::move(damage)),
         m_direct_strike(direct_strike)
   {
   }
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(
         m_attacked_nexus,
         m_attacking_player,
         m_direct_strike,
         m_damage,
         m_attacking_card,
         m_direct_strike);
   }
};
class NightfallEvent: public AnyEvent {
   PLAYER m_player;
   sptr< Card > m_card;

  public:
   static const EventType event_type = EventType::NIGHTFALL;
   NightfallEvent(PLAYER player, sptr< Card > card)
       : m_player(player), m_card(std::move(card))
   {
   }
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player, m_card);
   }
};
class PlayEvent: public AnyEvent {
   PLAYER m_player;
   sptr< Unit > m_card;

  public:
   static const EventType event_type = EventType::PLAY_UNIT;
   PlayEvent(PLAYER player, sptr< Unit > card)
       : m_player(player), m_card(std::move(card))
   {
   }
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player, m_card);
   }
};
class RecallEvent: public AnyEvent {
   PLAYER m_player;
   sptr< Card > m_card;

  public:
   static const EventType event_type = EventType::RECALL;
   RecallEvent(PLAYER player, sptr< Card > card)
       : m_player(player), m_card(std::move(card))
   {
   }
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player, m_card);
   }
};
class RoundEndEvent: public AnyEvent {
   size_t round;

  public:
   static const EventType event_type = EventType::ROUND_END;
   explicit RoundEndEvent(size_t round) : round(round) {}
   [[nodiscard]] auto get_event_data() const { return _get_event_data(round); }
};
class RoundStartEvent: public AnyEvent {
   size_t round;

  public:
   static const EventType event_type = EventType::ROUND_START;
   explicit RoundStartEvent(size_t round) : round(round) {}
   [[nodiscard]] auto get_event_data() const { return _get_event_data(round); }
};
class StrikeEvent: public AnyEvent {
   PLAYER m_player;
   sptr< Card > m_striker;

  public:
   static const EventType event_type = EventType::STRIKE;
   StrikeEvent(PLAYER player, sptr< Card > striker)
       : m_player(player), m_striker(std::move(striker))
   {
   }
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player, m_striker);
   }
};
class SummonEvent: public AnyEvent {
   PLAYER m_player;
   sptr< Card > m_card;

  public:
   static const EventType event_type = EventType::SUMMON;
   SummonEvent(PLAYER player, sptr< Card > card)
       : m_player(player), m_card(std::move(card))
   {
   }
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player, m_card);
   }
};
class StunEvent: public AnyEvent {
   PLAYER m_player;
   sptr< Card > m_card;

  public:
   static const EventType event_type = EventType::STUN;
   StunEvent(PLAYER player, sptr< Card > card)
       : m_player(player), m_card(std::move(card))
   {
   }
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player, m_card);
   }
};
class TargetEvent: public AnyEvent {
   PLAYER m_player;
   sptr< Card > m_targeted;

  public:
   static const EventType event_type = EventType::TARGET;
   TargetEvent(PLAYER player, sptr< Card > targeted)
       : m_player(player), m_targeted(std::move(targeted))
   {
   }
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player, m_targeted);
   }
};
class UnitTakeDamageEvent: public AnyEvent {
   PLAYER m_player;  // the player whose unit is damaged
   sptr< Unit > m_unit;
   sptr< size_t > m_damage;

  public:
   static const EventType event_type = EventType::UNIT_TAKE_DAMAGE;
   UnitTakeDamageEvent(PLAYER player, sptr< Unit > unit, sptr< size_t > damage)
       : m_player(player), m_unit(std::move(unit)), m_damage(std::move(damage))
   {
   }
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player, m_unit, m_damage);
   }
};
class UseManaEvent: public AnyEvent {
   PLAYER m_player;
   size_t m_amount;

  public:
   static const EventType event_type = EventType::USE_MANA;
   UseManaEvent(PLAYER player, size_t amount)
       : m_player(player), m_amount(amount)
   {
   }
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player, m_amount);
   }
};

// The most recent happening event
using VariantEvent = std::variant<
   AnyEvent,
   //   BattleEvent,
   AttackEvent,
   BeholdEvent,
   CastEvent,
   BlockEvent,
   DaybreakEvent,
   DieEvent,
   DiscardEvent,
   DrawCardEvent,
   EndRoundEvent,
   EnlightenmentEvent,
   GainManaEvent,
   GetCardEvent,
   LevelUpEvent,
   NexusStrikeEvent,
   NightfallEvent,
   PlayEvent,
   RecallEvent,
   RoundEndEvent,
   RoundStartEvent,
   StrikeEvent,
   StunEvent,
   SummonEvent,
   TargetEvent,
   UnitTakeDamageEvent,
   UseManaEvent >;

// visitors to access the data depending on the event
struct VisitorEventType {
   template < typename event_t >
   EventType operator()(const event_t& eve) const
   {
      return eve.event_type;
   }
};
struct VisitorEventData {
   template < typename event_t >
   auto operator()(const event_t& eve) const
   {
      return eve.get_event_data();
   }
};

EventType get_event_type(const VariantEvent& event)
{
   return std::visit(VisitorEventType{}, event);
}

}  // namespace events

#endif  // LORAINE_EVENT_H
