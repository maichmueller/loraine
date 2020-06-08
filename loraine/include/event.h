
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
class AnyEvent {
   static const EventType event_type;
};
class BattleEvent: public AnyEvent {
   static const EventType event_type = EventType::BATTLE;
   explicit BattleEvent() : AnyEvent() {}
};
class CastEvent: public AnyEvent {
   static const EventType event_type = EventType::CAST;
   SID m_player_id;
   UUID m_spell_id;
   bool m_is_spell;
   CastEvent(SID player_id, UUID spell_id, bool is_spell)
       : AnyEvent(),
         m_player_id(player_id),
         m_spell_id(spell_id),
         m_is_spell(is_spell)
   {
   }
   [[nodiscard]] auto get_player_id() const {return m_player_id;}
   [[nodiscard]] auto get_spell_id() const {return m_spell_id;}
   [[nodiscard]] auto is_spell() const {return m_is_spell;}
};
class DeclAttackEvent: public AnyEvent {
   static const EventType event_type = EventType::DECL_ATTACK;
   SID m_player_id;
   UUID m_attacker_id;
   u8 m_position;
   DeclAttackEvent(SID player_id, UUID attacker_id, u8 position)
       : AnyEvent(),
         m_player_id(player_id),
         m_attacker_id(attacker_id),
         m_position(position)
   {
   }
   [[nodiscard]] auto get_player_id() const {return m_player_id;}
   [[nodiscard]] auto get_attacker_id() const {return m_attacker_id;}
   [[nodiscard]] auto get_position() const {return m_position;}
};
class DeclBlockEvent: public AnyEvent {
   static const EventType event_type = EventType::DECL_BLOCK;
   SID m_player_id;
   UUID m_attacker_id;
   i8 m_position;
   DeclBlockEvent(SID player_id, UUID attacker_id, i8 position)
       : AnyEvent(),
         m_player_id(player_id),
         m_attacker_id(attacker_id),
         m_position(position)
   {
   }
   [[nodiscard]] auto get_player_id() const {return m_player_id;}
   [[nodiscard]] auto get_attacker_id() const {return m_attacker_id;}
   [[nodiscard]] auto is_spell() const {return m_position;}
};
class DieEvent: public AnyEvent {
   static const EventType event_type = EventType::DIE;
   SID m_player_id;
   CardID m_dead_card_id;
   DieEvent(SID player_id, CardID dead_card_id)
       : AnyEvent(), m_player_id(player_id), m_dead_card_id(dead_card_id)
   {
   }
   [[nodiscard]] auto get_player_id() const {return m_player_id;}
   [[nodiscard]] auto get_dead_card_id() const {return m_dead_card_id;}
};
class DrawCardEvent: public AnyEvent {
   static const EventType event_type = EventType::DRAW_CARD;
   SID m_player_id;
   CardID m_card_id;
   DrawCardEvent(SID player_id, CardID card_id)
       : AnyEvent(), m_player_id(player_id), m_card_id(card_id)
   {
   }
   [[nodiscard]] auto get_player_id() const {return m_player_id;}
   [[nodiscard]] auto get_card_id() const {return m_card_id;}
};
class EndRoundEvent: public AnyEvent {
   static const EventType event_type = EventType::END_ROUND;
};
class EnlightenmentEvent: public AnyEvent {
   static const EventType event_type = EventType::ENLIGHTENMENT;
};
class GainManaEvent: public AnyEvent {
   static const EventType event_type = EventType::GAIN_MANA;
   SID m_player_id;
   u8 m_amount;
   GainManaEvent(SID player_id, u8 amount)
       : AnyEvent(), m_player_id(player_id), m_amount(amount)
   {
   }
   [[nodiscard]] auto get_player_id() const {return m_player_id;}
   [[nodiscard]] auto get_amount() const {return m_amount;}
};
class GameEndEvent: public AnyEvent {
   static const EventType event_type = EventType::GAME_END;
   SID m_player_id;
   u8 m_amount;
   explicit GameEndEvent(SID player_id, u8 amount)
       : AnyEvent(), m_player_id(player_id), m_amount(amount)
   {
   }
   [[nodiscard]] auto get_player_id() const {return m_player_id;}
   [[nodiscard]] auto get_amount() const {return m_amount;}
};
class GetCardEvent: public AnyEvent {
   static const EventType event_type = EventType::GET_CARD;
   SID m_player_id;
   CardID m_card_id;
   GetCardEvent(SID player_id, CardID card_id)
       : AnyEvent(), m_player_id(player_id), m_card_id(card_id)
   {
   }
   [[nodiscard]] auto get_player_id() const {return m_player_id;}
   [[nodiscard]] auto get_card_id() const {return m_card_id;}
};
class LevelUpEvent: public AnyEvent {
   static const EventType event_type = EventType::LEVEL_UP;
   SID m_player_id;
   UUID m_card_id;
   LevelUpEvent(SID player_id, UUID card_id)
       : AnyEvent(), m_player_id(player_id), m_card_id(card_id)
   {
   }
   [[nodiscard]] auto get_player_id() const {return m_player_id;}
   [[nodiscard]] auto get_card_id() const {return m_card_id;}
};
class NexusStrikeEvent: public AnyEvent {
   static const EventType event_type = EventType::NEXUS_STRIKE;
   SID m_attacking_player;
   SID m_attacked_nexus_id;
   i64 m_damage;
   NexusStrikeEvent(SID attacking_player, SID attacked_nexus_id, i64 damage)
       : AnyEvent(),
         m_attacking_player(attacking_player),
         m_attacked_nexus_id(attacked_nexus_id),
         m_damage(damage)
   {
   }
   [[nodiscard]] auto get_attacking_player() const {return m_attacking_player;}
   [[nodiscard]] auto get_attacked_nexus_id() const {return m_attacked_nexus_id;}
   [[nodiscard]] auto get_damage() const {return m_damage;}
};
class NexusTakeDamageEvent: public AnyEvent {
   static const EventType event_type = EventType::NEXUS_DAMAGE;
   SID m_attacking_player;
   SID m_attacked_nexus_id;
   i64 m_damage;
   NexusTakeDamageEvent(SID attacking_player, SID attacked_nexus_id, i64 damage)
       : AnyEvent(),
         m_attacking_player(attacking_player),
         m_attacked_nexus_id(attacked_nexus_id),
         m_damage(damage)
   {
   }
   [[nodiscard]] auto get_attacking_player() const {return m_attacking_player;}
   [[nodiscard]] auto get_attacked_nexus_id() const {return m_attacked_nexus_id;}
   [[nodiscard]] auto get_damage() const {return m_damage;}
};
class PlayEvent: public AnyEvent {
   static const EventType event_type = EventType::PLAY_UNIT;
   SID m_player_id;
   UUID m_card_id;
   PlayEvent(SID player_id, UUID card_id)
       : AnyEvent(), m_player_id(player_id), m_card_id(card_id)
   {
   }
   [[nodiscard]] auto get_player_id() const {return m_player_id;}
   [[nodiscard]] auto get_card_id() const {return m_card_id;}
};
class PlaySpell: public AnyEvent {
   static const EventType event_type = EventType::PLAY_SPELL;
   SID m_player_id;
   UUID m_spell_id;
   bool m_is_spell;
   PlaySpell(SID player_id, UUID card_id, bool is_skill)
       : AnyEvent(), m_player_id(player_id), m_spell_id(card_id), m_is_spell(is_skill)
   {
   }
   [[nodiscard]] auto get_player_id() const {return m_player_id;}
   [[nodiscard]] auto get_spell_id() const {return m_spell_id;}
   [[nodiscard]] auto is_spell() const {return m_is_spell;}
};
class RecallEvent: public AnyEvent {
   static const EventType event_type = EventType::RECALL;
   SID m_player_id;
   UUID m_card_id;
   RecallEvent(SID player_id, UUID card_id)
       : AnyEvent(), m_player_id(player_id), m_card_id(card_id)
   {
   }
   [[nodiscard]] auto get_player_id() const {return m_player_id;}
   [[nodiscard]] auto get_card_id() const {return m_card_id;}
};
class StartRoundEvent: public AnyEvent {
   static const EventType event_type = EventType::START_ROUND;
   u16 round;
   explicit StartRoundEvent(u16 round) : AnyEvent(), round(round) {}
};
class StrikeEvent: public AnyEvent {
   static const EventType event_type = EventType::STRIKE;
   SID m_player_id;
   UUID m_striker_id;
   StrikeEvent(SID player_id, UUID striker_id)
       : AnyEvent(), m_player_id(player_id), m_striker_id(striker_id)
   {
   }
   [[nodiscard]] auto get_player_id() const {return m_player_id;}
   [[nodiscard]] auto get_striker_id() const {return m_striker_id;}
};
class SummonEvent: public AnyEvent {
   static const EventType event_type = EventType::SUMMON;
   SID m_player_id;
   UUID m_card_id;
   SummonEvent(SID player_id, UUID card_id)
       : AnyEvent(), m_player_id(player_id), m_card_id(card_id)
   {
   }
   [[nodiscard]] auto get_player_id() const {return m_player_id;}
   [[nodiscard]] auto get_card_id() const {return m_card_id;}
};
class StunEvent: public AnyEvent {
   static const EventType event_type = EventType::STUN;
   SID m_player_id;
   UUID m_card_id;
   StunEvent(SID player_id, UUID card_id)
       : AnyEvent(), m_player_id(player_id), m_card_id(card_id)
   {
   }
   [[nodiscard]] auto get_player_id() const {return m_player_id;}
   [[nodiscard]] auto get_card_id() const {return m_card_id;}
};
class TargetEvent: public AnyEvent {
   static const EventType event_type = EventType::TARGET;
   SID m_player_id;
   UUID m_targeted_id;
   TargetEvent(SID player_id, UUID targeted_id)
       : AnyEvent(), m_player_id(player_id), m_targeted_id(targeted_id)
   {
   }
   [[nodiscard]] auto get_player_id() const {return m_player_id;}
   [[nodiscard]] auto get_targeted_id() const {return m_targeted_id;}
};
class UnitTakeDamageEvent: public AnyEvent {
   static const EventType event_type = EventType::UNIT_TAKE_DAMAGE;
   SID m_player_id;
   UUID m_targeted_id;
   i64 m_damage;
   UnitTakeDamageEvent(SID player_id, UUID targeted_id, i64 damage)
       : AnyEvent(),
         m_player_id(player_id),
         m_targeted_id(targeted_id),
         m_damage(damage)
   {
   }
   [[nodiscard]] auto get_player_id() const {return m_player_id;}
   [[nodiscard]] auto get_targeted_id() const {return m_targeted_id;}
   [[nodiscard]] auto get_damage() const {return m_damage;}
};
class UseManaEvent: public AnyEvent {
   static const EventType event_type = EventType::USE_MANA;
   SID m_player_id;
   u8 m_amount;
   UseManaEvent(SID player_id, u8 amount)
       : AnyEvent(), m_player_id(player_id), m_amount(amount)
   {
   }
   [[nodiscard]] auto get_player_id() const {return m_player_id;}
   [[nodiscard]] auto get_amount() const {return m_amount;}
};

// The most recent happening event
using VariantEvent = std::variant<
   AnyEvent,
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

}  // namespace event

#endif  // LORAINE_EVENT_H
