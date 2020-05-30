//
// Created by michael on 30.05.20.
//

#ifndef LORAINE_EVENT_H
#define LORAINE_EVENT_H

#define EVENT_MAX_SIZE 6

#include <variant>

#include "types.h"

namespace event {

enum struct EventType {
   ATTACK,  // important for e.g. Quick-Attack determination
   BLOCK,
   CAST,
   DECL_ATTACK,
   DECL_BLOCK,
   DIE,
   DRAW_CARD,
   END_ROUND,
   ENLIGHTEN,
   GAIN_MANA,
   USE_MANA,
   GAME_END,
   GET_CARD,
   UNIT_TAKE_DAMAGE,
   LEVEL_UP,
   NEXUS_DAMAGE,  // from spells or skills
   NEXUS_STRIKE,  // from direct attack
   PLAY_UNIT,
   PLAY_SPELL,
   RECALL,
   START_ROUND,
   STRIKE,
   STUN,
   SUMMON,
   TARGET,
};
struct AnyEvent {
   EventType type;
};
struct AttackEvent: public AnyEvent {
   EventType type = EventType::ATTACK;
   SID player_id;
   explicit AttackEvent(SID player_id) : AnyEvent(), player_id(player_id) {}
};
struct BlockEvent: public AnyEvent {
   EventType type = EventType::BLOCK;
   SID player_id;
   explicit BlockEvent(SID player_id) : AnyEvent(), player_id(player_id) {}
};
struct CastEvent: public AnyEvent {
   EventType type = EventType::CAST;
   SID player_id;
   CardID spell_id;
   bool is_spell;
   CastEvent(SID player_id, CardID spell_id, bool is_spell)
       : AnyEvent(),
         player_id(player_id),
         spell_id(spell_id),
         is_spell(is_spell)
   {
   }
};

struct DeclAttackEvent: public AnyEvent {
   EventType type = EventType::DECL_ATTACK;
   SID player_id;
   SID attacker_id;
   u8 position;
   DeclAttackEvent(SID player_id, SID attacker_id, u8 position)
       : AnyEvent(),
         player_id(player_id),
         attacker_id(attacker_id),
         position(position)
   {
   }
};
struct DeclBlockEvent: public AnyEvent {
   EventType type = EventType::DECL_BLOCK;
   SID player_id;
   SID attacker_id;
   i8 position;
   DeclBlockEvent(SID player_id, SID attacker_id, i8 position)
       : AnyEvent(),
         player_id(player_id),
         attacker_id(attacker_id),
         position(position)
   {
   }
};
struct DieEvent: public AnyEvent {
   EventType type = EventType::DIE;
   SID player_id;
   SID dead_id;
   DieEvent(SID player_id, SID dead_id)
       : AnyEvent(), player_id(player_id), dead_id(dead_id)
   {
   }
};
struct DrawCardEvent: public AnyEvent {
   EventType type = EventType::DRAW_CARD;
   SID player_id;
   CardID card_id;
   DrawCardEvent(SID player_id, CardID card_id)
       : AnyEvent(), player_id(player_id), card_id(card_id)
   {
   }
};
struct EndRoundEvent: public AnyEvent {
   EventType type = EventType::END_ROUND;
};
struct EnlightenEvent: public AnyEvent {
   EventType type = EventType::ENLIGHTEN;
};
struct GetCardEvent: public AnyEvent {
   EventType type = EventType::GET_CARD;
   SID player_id;
   CardID card_id;
   GetCardEvent(SID player_id, CardID card_id)
       : AnyEvent(), player_id(player_id), card_id(card_id)
   {
   }
};
struct LevelUpEvent: public AnyEvent {
   EventType type = EventType::LEVEL_UP;
   SID player_id;
   CardID card_id;
   LevelUpEvent(SID player_id, CardID card_id)
       : AnyEvent(), player_id(player_id), card_id(card_id)
   {
   }
};
struct NexusStrikeEvent: public AnyEvent {
   EventType type = EventType::NEXUS_STRIKE;
   SID attacking_player;
   SID attacked_nexus_id;
   i64 damage;
   NexusStrikeEvent(SID attacking_player, SID attacked_nexus_id, i64 damage)
       : AnyEvent(),
         attacking_player(attacking_player),
         attacked_nexus_id(attacked_nexus_id),
         damage(damage)
   {
   }
};
struct PlayEvent: public AnyEvent {
   EventType type = EventType::PLAY_UNIT;
   SID player_id;
   SID card_id;
   PlayEvent(SID player_id, SID card_id)
       : AnyEvent(), player_id(player_id), card_id(card_id)
   {
   }
};
struct PlaySpell: public AnyEvent {
   EventType type = EventType::PLAY_SPELL;
   SID player_id;
   CardID card_id;
   bool is_skill;
   PlaySpell(SID player_id, CardID card_id, bool is_skill)
       : AnyEvent(), player_id(player_id), card_id(card_id), is_skill(is_skill)
   {
   }
};
struct StartRoundEvent: public AnyEvent {
   EventType type = EventType::START_ROUND;
   u16 round;
   explicit StartRoundEvent(u16 round) : AnyEvent(), round(round) {}
};
struct StrikeEvent: public AnyEvent {
   EventType type = EventType::STRIKE;
   SID player_id;
   SID striker_id;
   StrikeEvent(SID player_id, SID striker_id)
       : AnyEvent(), player_id(player_id), striker_id(striker_id)
   {
   }
};
struct SummonEvent: public AnyEvent {
   EventType type = EventType::SUMMON;
   SID player_id;
   SID card_id;
   SummonEvent(SID player_id, SID card_id)
       : AnyEvent(), player_id(player_id), card_id(card_id)
   {
   }
};
struct TargetEvent: public AnyEvent {
   EventType type = EventType::TARGET;
   SID player_id;
   SID targeted_id;
   TargetEvent(SID player_id, SID targeted_id)
       : AnyEvent(), player_id(player_id), targeted_id(targeted_id)
   {
   }
};

// The most recent happening event. Different processes should be triggered
// depending on the type of event.
static std::variant<
   AnyEvent,
   CastEvent,
   DeclAttackEvent,
   DeclBlockEvent,
   DieEvent,
   DrawCardEvent,
   EndRoundEvent,
   EnlightenEvent,
   GetCardEvent,
   LevelUpEvent,
   NexusStrikeEvent,
   PlayEvent,
   PlaySpell,
   StartRoundEvent,
   StrikeEvent,
   SummonEvent,
   TargetEvent >
   active_event;

}  // namespace event

#endif  // LORAINE_EVENT_H
