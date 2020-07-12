
#ifndef LORAINE_EVENT_H
#define LORAINE_EVENT_H

#include <stack>
#include <variant>
#include <vector>

#include "rulesets.h"
#include "types.h"

namespace events {

enum class EventType {
   NONE,
   BATTLE,  // battle commences
   CAST,  // casting spells/skills (activation event, not committing event)
   DECL_ATTACK,  // declaring attacker
   DECL_BLOCK,  // declaring blockers
   DIE,  // a unit dies
   DISCARD,  // removing a card from hand (not playing)
   DRAW_CARD,  // draw a card (own deck or enemy deck)
   END_ROUND,  // the round ends
   ENLIGHTENMENT,  // reaching 10 mana gems
   GAIN_MANAGEM,  // gaining an amount of mana (e.g. round-start)
   GAME_END,  // a player won
   GET_CARD,  // get a cards from
   LEVEL_UP,  // champion levels up
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

struct AnyEvent {
   static const EventType event_type = EventType::NONE;
   /*
    * The base class method to return all base member variables
    */
   [[nodiscard]] auto get_event_data() const { return std::tuple{}; }

  protected:
   /*
    * This is a protected stack of methods to allow the child classes to simply
    * call _get_event_data() in their own public get_event_data() methods
    * without needing to constantly copy over arguments from the base class into
    * the method call. This will be done automatically by the methods below!
    */
   template < typename Tuple, size_t... I, typename... Args >
   [[nodiscard]] auto _get_event_data_base(
      Tuple t, std::index_sequence< I... >, Args... args) const
   {
      return std::tuple{std::get< I >(t)..., args...};
   }
   template < typename Tuple, typename... Args >
   auto _get_event_data_base_applied(Tuple t, Args... args) const
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
      return std::tuple{get_event_data(), args...};
   }

  public:
   virtual ~AnyEvent() = default;
   AnyEvent() {}
};
struct BattleEvent: public AnyEvent {
   static const EventType event_type = EventType::BATTLE;
   explicit BattleEvent() : AnyEvent() {}
};
class CastEvent: public AnyEvent {
   PLAYER m_player;
   UUID m_spell_id;
   bool m_is_spell;

  public:
   static const EventType event_type = EventType::CAST;
   CastEvent(PLAYER player, UUID spell_id, bool is_spell)
       : m_player(player), m_spell_id(spell_id), m_is_spell(is_spell)
   {
   }
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player, m_spell_id, m_is_spell);
   }
};
class DeclAttackEvent: public AnyEvent {
   PLAYER m_player;
   UUID m_attacker_id;
   size_t m_position;

  public:
   static const EventType event_type = EventType::DECL_ATTACK;
   DeclAttackEvent(PLAYER player, UUID attacker_id, size_t position)
       : m_player(player), m_attacker_id(attacker_id), m_position(position)
   {
   }
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player, m_attacker_id, m_position);
   }
};
class DeclBlockEvent: public AnyEvent {
   PLAYER m_player;
   UUID m_attacker_id;
   i32 m_position;

  public:
   static const EventType event_type = EventType::DECL_BLOCK;
   DeclBlockEvent(PLAYER player, UUID attacker_id, i32 position)
       : m_player(player), m_attacker_id(attacker_id), m_position(position)
   {
   }
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player, m_attacker_id, m_position);
   }
};
class DieEvent: public AnyEvent {
   PLAYER m_player;
   CardID m_dead_card_id;

  public:
   static const EventType event_type = EventType::DIE;
   DieEvent(PLAYER player, CardID dead_card_id)
       : m_player(player), m_dead_card_id(dead_card_id)
   {
   }
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player, m_dead_card_id);
   }
};
class DiscardEvent: public AnyEvent {
   PLAYER m_player;
   CardID m_card_id;
   UUID m_card_uuid;

  public:
   static const EventType event_type = EventType::DISCARD;
   DiscardEvent(PLAYER player, CardID card_id, UUID card_uuid)
       : m_player(player), m_card_id(card_id), m_card_uuid(card_uuid)
   {
   }
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player, m_card_id, m_card_uuid);
   }
};
class DrawCardEvent: public AnyEvent {
   PLAYER m_player;
   CardID m_card_id;

  public:
   static const EventType event_type = EventType::DRAW_CARD;
   DrawCardEvent(PLAYER player, CardID card_id)
       : m_player(player), m_card_id(card_id)
   {
   }
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player, m_card_id);
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
class GameEndEvent: public AnyEvent {
   PLAYER m_player;
   size_t m_amount;

  public:
   static const EventType event_type = EventType::GAME_END;
   explicit GameEndEvent(PLAYER player, size_t amount)
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
   UUID m_card_id;

  public:
   static const EventType event_type = EventType::LEVEL_UP;
   LevelUpEvent(PLAYER player, UUID card_id)
       : m_player(player), m_card_id(card_id)
   {
   }
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player, m_card_id);
   }
};
class NexusStrikeEvent: public AnyEvent {
   SID m_attacked_nexus;
   PLAYER m_attacking_player;
   UUID m_attacking_card_uuid;
   SymArr< i32 > m_damage;
   bool m_direct_strike;  // whether the attack was from hitting the
                          // nexus in battle or through effects/spells

  public:
   static const EventType event_type = EventType::NEXUS_STRIKE;
   NexusStrikeEvent(
      SID attacked_nexus,
      UUID attacking_card_uuid,
      PLAYER attacking_player,
      SymArr< i32 > damage,
      bool direct_strike)
       : m_attacked_nexus(attacked_nexus),
         m_attacking_player(attacking_player),
         m_attacking_card_uuid(attacking_card_uuid),
         m_damage(damage),
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
         m_attacking_card_uuid,
         m_direct_strike);
   }
};
class PlayEvent: public AnyEvent {
   PLAYER m_player;
   UUID m_card_id;

  public:
   static const EventType event_type = EventType::PLAY_UNIT;
   PlayEvent(PLAYER player, UUID card_id) : m_player(player), m_card_id(card_id)
   {
   }
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player, m_card_id);
   }
};
class PlaySpell: public AnyEvent {
   PLAYER m_player;
   UUID m_spell_id;
   bool m_is_spell;

  public:
   static const EventType event_type = EventType::PLAY_SPELL;
   PlaySpell(PLAYER player, UUID card_id, bool is_skill)
       : m_player(player), m_spell_id(card_id), m_is_spell(is_skill)
   {
   }
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player, m_spell_id, m_is_spell);
   }
};
class RecallEvent: public AnyEvent {
   PLAYER m_player;
   UUID m_card_id;

  public:
   static const EventType event_type = EventType::RECALL;
   RecallEvent(PLAYER player, UUID card_id)
       : m_player(player), m_card_id(card_id)
   {
   }
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player, m_card_id);
   }
};
class StartRoundEvent: public AnyEvent {
   size_t round;

  public:
   static const EventType event_type = EventType::START_ROUND;
   explicit StartRoundEvent(size_t round) : round(round) {}
   [[nodiscard]] auto get_event_data() const { return _get_event_data(round); }
};
class StrikeEvent: public AnyEvent {
   PLAYER m_player;
   UUID m_striker_id;

  public:
   static const EventType event_type = EventType::STRIKE;
   StrikeEvent(PLAYER player, UUID striker_id)
       : m_player(player), m_striker_id(striker_id)
   {
   }
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player, m_striker_id);
   }
};
class SummonEvent: public AnyEvent {
   PLAYER m_player;
   UUID m_card_id;

  public:
   static const EventType event_type = EventType::SUMMON;
   SummonEvent(PLAYER player, UUID card_id)
       : m_player(player), m_card_id(card_id)
   {
   }
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player, m_card_id);
   }
};
class StunEvent: public AnyEvent {
   PLAYER m_player;
   UUID m_card_id;

  public:
   static const EventType event_type = EventType::STUN;
   StunEvent(PLAYER player, UUID card_id) : m_player(player), m_card_id(card_id)
   {
   }
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player, m_card_id);
   }
};
class TargetEvent: public AnyEvent {
   PLAYER m_player;
   UUID m_targeted_id;

  public:
   static const EventType event_type = EventType::TARGET;
   TargetEvent(PLAYER player, UUID targeted_id)
       : m_player(player), m_targeted_id(targeted_id)
   {
   }
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player, m_targeted_id);
   }
};
class UnitTakeDamageEvent: public AnyEvent {
   PLAYER m_player;
   UUID m_targeted_id;
   size_t m_damage;

  public:
   static const EventType event_type = EventType::UNIT_TAKE_DAMAGE;
   UnitTakeDamageEvent(PLAYER player, UUID targeted_id, size_t damage)
       : m_player(player), m_targeted_id(targeted_id), m_damage(damage)
   {
   }
   [[nodiscard]] auto get_event_data() const
   {
      return _get_event_data(m_player, m_targeted_id, m_damage);
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
   BattleEvent,
   CastEvent,
   DeclAttackEvent,
   DeclBlockEvent,
   DieEvent,
   DiscardEvent,
   DrawCardEvent,
   EndRoundEvent,
   EnlightenmentEvent,
   GainManaEvent,
   GameEndEvent,
   GetCardEvent,
   LevelUpEvent,
   NexusStrikeEvent,
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

struct active_event {
   static void set(AnyEvent && event) {
      m_active_event = std::move(event);
//      events::Event
   }
   static const VariantEvent & get() {
      return m_active_event;
   }
  private:
   static VariantEvent m_active_event;

};
}  // namespace events

#endif  // LORAINE_EVENT_H
