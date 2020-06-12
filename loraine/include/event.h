
#ifndef LORAINE_EVENT_H
#define LORAINE_EVENT_H

#include <stack>
#include <variant>
#include <vector>

#include "types.h"

namespace events {

enum class EventType {
   BATTLE,  // battle commences
   CAST,  // casting spells/skills (activation event, not committing event)
   DECL_ATTACK,  // declaring attacker
   DECL_BLOCK,  // declaring blockers
   DIE,  // a unit dies
   DRAW_CARD,  // draw a cards (own deck or enemy deck)
   END_ROUND,  // the round ends
   ENLIGHTENMENT,  // reaching 10 mana gems
   GAIN_MANA,  // gaining an amount of mana (e.g. round-start)
   GAME_END,  // a player won
   GET_CARD,  // get a cards from
   LEVEL_UP,  // champion levels up
   NEXUS_DAMAGE,  // from spells or skills
   NEXUS_STRIKE,  // from direct attack
   PLAY_UNIT,  // playing a unit from hand onto the field
   PLAY_SPELL,  // playing a spell from hand
   RECALL,  // a unit gets recalled from field back to the hand
   START_ROUND,  // the round starts
   STRIKE,  // a unit strikes (anything)
   STUN,  // stunning a unit
   SUMMON,  // summoning a unit (subset of playing a unit)
   TARGET,  // targeting a unit with a spell/skill (
            // 'targeting' effects only occur, when the unit is actually hit)
            // 'being targeted' effects only occur, when the unit survives)
   UNIT_TAKE_DAMAGE,  // any unit on the field takes damage
   USE_MANA,  // using mana (play units, cast spells etc.)
};

struct BattleEvent {
   static const EventType event_type = EventType::BATTLE;
   BattleEvent() = default;
};
class CastEvent {
   SID m_player_id;
   UUID m_spell_id;
   bool m_is_spell;

  public:
   static const EventType event_type = EventType::CAST;
   CastEvent(SID player_id, UUID spell_id, bool is_spell)
       : m_player_id(player_id), m_spell_id(spell_id), m_is_spell(is_spell)
   {
   }
   [[nodiscard]] auto get_player_id() const { return m_player_id; }
   [[nodiscard]] auto get_spell_id() const { return m_spell_id; }
   [[nodiscard]] auto is_spell() const { return m_is_spell; }
};
class DeclAttackEvent {
   SID m_player_id;
   UUID m_attacker_id;
   u8 m_position;

  public:
   static const EventType event_type = EventType::DECL_ATTACK;
   DeclAttackEvent(SID player_id, UUID attacker_id, u8 position)
       : m_player_id(player_id),
         m_attacker_id(attacker_id),
         m_position(position)
   {
   }
   [[nodiscard]] auto get_player_id() const { return m_player_id; }
   [[nodiscard]] auto get_attacker_id() const { return m_attacker_id; }
   [[nodiscard]] auto get_position() const { return m_position; }
};
class DeclBlockEvent {
   SID m_player_id;
   UUID m_attacker_id;
   i8 m_position;

  public:
   static const EventType event_type = EventType::DECL_BLOCK;
   DeclBlockEvent(SID player_id, UUID attacker_id, i8 position)
       : m_player_id(player_id),
         m_attacker_id(attacker_id),
         m_position(position)
   {
   }
   [[nodiscard]] auto get_player_id() const { return m_player_id; }
   [[nodiscard]] auto get_attacker_id() const { return m_attacker_id; }
   [[nodiscard]] auto is_spell() const { return m_position; }
};
class DieEvent {
   SID m_player_id;
   CardID m_dead_card_id;

  public:
   static const EventType event_type = EventType::DIE;
   DieEvent(SID player_id, CardID dead_card_id)
       : m_player_id(player_id), m_dead_card_id(dead_card_id)
   {
   }
   [[nodiscard]] auto get_player_id() const { return m_player_id; }
   [[nodiscard]] auto get_dead_card_id() const { return m_dead_card_id; }
};
class DrawCardEvent {
   SID m_player_id;
   CardID m_card_id;

  public:
   static const EventType event_type = EventType::DRAW_CARD;
   DrawCardEvent(SID player_id, CardID card_id)
       : m_player_id(player_id), m_card_id(card_id)
   {
   }
   [[nodiscard]] auto get_player_id() const { return m_player_id; }
   [[nodiscard]] auto get_card_id() const { return m_card_id; }
};
struct EndRoundEvent {
   static const EventType event_type = EventType::END_ROUND;
};
struct EnlightenmentEvent {
   static const EventType event_type = EventType::ENLIGHTENMENT;
};
class GainManaEvent {
   SID m_player_id;
   u8 m_amount;

  public:
   static const EventType event_type = EventType::GAIN_MANA;
   GainManaEvent(SID player_id, u8 amount)
       : m_player_id(player_id), m_amount(amount)
   {
   }
   [[nodiscard]] auto get_player_id() const { return m_player_id; }
   [[nodiscard]] auto get_amount() const { return m_amount; }
};
class GameEndEvent {
   SID m_player_id;
   u8 m_amount;

  public:
   static const EventType event_type = EventType::GAME_END;
   explicit GameEndEvent(SID player_id, u8 amount)
       : m_player_id(player_id), m_amount(amount)
   {
   }
   [[nodiscard]] auto get_player_id() const { return m_player_id; }
   [[nodiscard]] auto get_amount() const { return m_amount; }
};
class GetCardEvent {
   SID m_player_id;
   CardID m_card_id;

  public:
   static const EventType event_type = EventType::GET_CARD;
   GetCardEvent(SID player_id, CardID card_id)
       : m_player_id(player_id), m_card_id(card_id)
   {
   }
   [[nodiscard]] auto get_player_id() const { return m_player_id; }
   [[nodiscard]] auto get_card_id() const { return m_card_id; }
};
class LevelUpEvent {
   SID m_player_id;
   UUID m_card_id;

  public:
   static const EventType event_type = EventType::LEVEL_UP;
   LevelUpEvent(SID player_id, UUID card_id)
       : m_player_id(player_id), m_card_id(card_id)
   {
   }
   [[nodiscard]] auto get_player_id() const { return m_player_id; }
   [[nodiscard]] auto get_card_id() const { return m_card_id; }
};
class NexusStrikeEvent {
   SID m_attacking_player;
   SID m_attacked_nexus_id;
   i64 m_damage;

  public:
   static const EventType event_type = EventType::NEXUS_STRIKE;
   NexusStrikeEvent(SID attacking_player, SID attacked_nexus_id, i64 damage)
       : m_attacking_player(attacking_player),
         m_attacked_nexus_id(attacked_nexus_id),
         m_damage(damage)
   {
   }
   [[nodiscard]] auto get_attacking_player() const
   {
      return m_attacking_player;
   }
   [[nodiscard]] auto get_attacked_nexus_id() const
   {
      return m_attacked_nexus_id;
   }
   [[nodiscard]] auto get_damage() const { return m_damage; }
};
class NexusTakeDamageEvent {
   SID m_attacking_player;
   SID m_attacked_nexus_id;
   i64 m_damage;

  public:
   static const EventType event_type = EventType::NEXUS_DAMAGE;
   NexusTakeDamageEvent(SID attacking_player, SID attacked_nexus_id, i64 damage)
       : m_attacking_player(attacking_player),
         m_attacked_nexus_id(attacked_nexus_id),
         m_damage(damage)
   {
   }
   [[nodiscard]] auto get_attacking_player() const
   {
      return m_attacking_player;
   }
   [[nodiscard]] auto get_attacked_nexus_id() const
   {
      return m_attacked_nexus_id;
   }
   [[nodiscard]] auto get_damage() const { return m_damage; }
};
class PlayEvent {
   SID m_player_id;
   UUID m_card_id;

  public:
   static const EventType event_type = EventType::PLAY_UNIT;
   PlayEvent(SID player_id, UUID card_id)
       : m_player_id(player_id), m_card_id(card_id)
   {
   }
   [[nodiscard]] auto get_player_id() const { return m_player_id; }
   [[nodiscard]] auto get_card_id() const { return m_card_id; }
};
class PlaySpell {
   SID m_player_id;
   UUID m_spell_id;
   bool m_is_spell;

  public:
   static const EventType event_type = EventType::PLAY_SPELL;
   PlaySpell(SID player_id, UUID card_id, bool is_skill)
       : m_player_id(player_id), m_spell_id(card_id), m_is_spell(is_skill)
   {
   }
   [[nodiscard]] auto get_player_id() const { return m_player_id; }
   [[nodiscard]] auto get_spell_id() const { return m_spell_id; }
   [[nodiscard]] auto is_spell() const { return m_is_spell; }
};
class RecallEvent {
   SID m_player_id;
   UUID m_card_id;

  public:
   static const EventType event_type = EventType::RECALL;
   RecallEvent(SID player_id, UUID card_id)
       : m_player_id(player_id), m_card_id(card_id)
   {
   }
   [[nodiscard]] auto get_player_id() const { return m_player_id; }
   [[nodiscard]] auto get_card_id() const { return m_card_id; }
};
class StartRoundEvent {
   u16 round;
  public:
   static const EventType event_type = EventType::START_ROUND;
   explicit StartRoundEvent(u16 round) : round(round) {}
};
class StrikeEvent {
   SID m_player_id;
   UUID m_striker_id;

  public:
   static const EventType event_type = EventType::STRIKE;
   StrikeEvent(SID player_id, UUID striker_id)
       : m_player_id(player_id), m_striker_id(striker_id)
   {
   }
   [[nodiscard]] auto get_player_id() const { return m_player_id; }
   [[nodiscard]] auto get_striker_id() const { return m_striker_id; }
};
class SummonEvent {
   SID m_player_id;
   UUID m_card_id;

  public:
   static const EventType event_type = EventType::SUMMON;
   SummonEvent(SID player_id, UUID card_id)
       : m_player_id(player_id), m_card_id(card_id)
   {
   }
   [[nodiscard]] auto get_player_id() const { return m_player_id; }
   [[nodiscard]] auto get_card_id() const { return m_card_id; }
};
class StunEvent {
   SID m_player_id;
   UUID m_card_id;

  public:
   static const EventType event_type = EventType::STUN;
   StunEvent(SID player_id, UUID card_id)
       : m_player_id(player_id), m_card_id(card_id)
   {
   }
   [[nodiscard]] auto get_player_id() const { return m_player_id; }
   [[nodiscard]] auto get_card_id() const { return m_card_id; }
};
class TargetEvent {
   SID m_player_id;
   UUID m_targeted_id;

  public:
   static const EventType event_type = EventType::TARGET;
   TargetEvent(SID player_id, UUID targeted_id)
       : m_player_id(player_id), m_targeted_id(targeted_id)
   {
   }
   [[nodiscard]] auto get_player_id() const { return m_player_id; }
   [[nodiscard]] auto get_targeted_id() const { return m_targeted_id; }
};
class UnitTakeDamageEvent {
   SID m_player_id;
   UUID m_targeted_id;
   i64 m_damage;

  public:
   static const EventType event_type = EventType::UNIT_TAKE_DAMAGE;
   UnitTakeDamageEvent(SID player_id, UUID targeted_id, i64 damage)
       : m_player_id(player_id), m_targeted_id(targeted_id), m_damage(damage)
   {
   }
   [[nodiscard]] auto get_player_id() const { return m_player_id; }
   [[nodiscard]] auto get_targeted_id() const { return m_targeted_id; }
   [[nodiscard]] auto get_damage() const { return m_damage; }
};
class UseManaEvent {
   SID m_player_id;
   u8 m_amount;

  public:
   static const EventType event_type = EventType::USE_MANA;
   UseManaEvent(SID player_id, u8 amount)
       : m_player_id(player_id), m_amount(amount)
   {
   }
   [[nodiscard]] auto get_player_id() const { return m_player_id; }
   [[nodiscard]] auto get_amount() const { return m_amount; }
};

// The most recent happening event
using VariantEvent = std::variant<
   BattleEvent,
   CastEvent,
   DeclAttackEvent,
   DeclBlockEvent,
   DieEvent,
   DrawCardEvent,
   EndRoundEvent,
   EnlightenmentEvent,
   GainManaEvent,
   GameEndEvent,
   GetCardEvent,
   LevelUpEvent,
   NexusStrikeEvent,
   NexusTakeDamageEvent,
   PlayEvent,
   PlaySpell,
   RecallEvent,
   StartRoundEvent,
   StrikeEvent,
   StunEvent,
   SummonEvent,
   TargetEvent,
   UnitTakeDamageEvent,
   UseManaEvent >;

static VariantEvent last_event;

// visitors to access the data depending on the event
struct VisitorEventType {
   template < typename event_t >
   EventType operator()(const event_t& eve) const
   {
      return eve.event_type;
   }
};
struct VisitorPlayerID {
   template < typename event_t >
   SID operator()(const event_t& eve) const
   {
      return eve.get_player_id();
   }
};
struct VisitorTargetedID {
   template < typename event_t >
   UUID operator()(const event_t& eve) const
   {
      return eve.get_targeted_id();
   }
};
struct VisitorStrikerID {
   template < typename event_t >
   SID operator()(const event_t& eve) const
   {
      return eve.get_striker_id();
   }
};
struct VisitorCardID {
   template < typename event_t >
   UUID operator()(const event_t& eve) const
   {
      return eve.get_card_id();
   }
};
struct VisitorDeadCardID {
   template < typename event_t >
   UUID operator()(const event_t& eve) const
   {
      return eve.get_dead_card_id();
   }
};
struct VisitorDamage {
   template < typename event_t >
   u8 operator()(const event_t& eve) const
   {
      return eve.get_damage();
   }
};
struct VisitorAmount {
   template < typename event_t >
   u8 operator()(const event_t& eve) const
   {
      return eve.get_amount();
   }
};
struct VisitorIsSpell {
   template < typename event_t >
   bool operator()(const event_t& eve) const
   {
      return eve.is_spell();
   }
};
struct VisitorSpellID {
   template < typename event_t >
   UUID operator()(const event_t& eve) const
   {
      return eve.get_spell_id();
   }
};
struct VisitorAttackerID {
   template < typename event_t >
   UUID operator()(const event_t& eve) const
   {
      return eve.get_attacker_id();
   }
};
struct VisitorPosition {
   template < typename event_t >
   u8 operator()(const event_t& eve) const
   {
      return eve.get_attacker_id();
   }
};
struct VisitorAttackingPlayer {
   template < typename event_t >
   SID operator()(const event_t& eve) const
   {
      return eve.get_attacking_player();
   }
};
struct VisitorAttackedNexusID {
   template < typename event_t >
   SID operator()(const event_t& eve) const
   {
      return eve.get_attacked_nexus_id();
   }
};

EventType get_event_type(const VariantEvent& event)
{
   return std::visit(VisitorEventType{}, event);
}
SID get_player_id(const VariantEvent& event)
{
   return std::visit(VisitorPlayerID{}, event);
}
//SID get_player_id(const VariantEvent& event)
//{
//   return std::visit(VisitorPlayerID{}, event);
//}

}  // namespace events

#endif  // LORAINE_EVENT_H
