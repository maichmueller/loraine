

#ifndef LORAINE_EVENT_ID_H
#define LORAINE_EVENT_ID_H

namespace events {

enum class EventID {
   ATTACK = 0,  // declaring attacker
   BEHOLD,  // the team has an 8+ mana unit in hand or in play_event_triggers
   BLOCK, // a unit blocked an attacking unit
   CAPTURE,  // A unit has captured another unit
   CAST,  // casting spells/skills (activation m_subscribed_events, not committing m_subscribed_events)
   DAYBREAK,  // the played spell is the first to be played in this round
   DISCARD,  // removing a spell from hand (not playing)
   DRAW_CARD,  // draw a spell (own deck or enemy deck)
   ENLIGHTENMENT,  // reaching 10 mana gems
   GAIN_MANAGEM,  // gaining an amount of mana (e.g. round-start)
   HEAL_UNIT,  // healing a unit for some amount
   LEVEL_UP,  // champion levels up
   NEXUS_DAMAGE,  // from non-combat attack
   NEXUS_STRIKE,  // from combat attack
   NIGHTFALL,  // the played spell is not the first to be played in this round
   PLAY,  // summoning a unit from hand or playing a floating
   RECALL,  // a unit gets recalled from field back to the hand
   ROUND_END,  // the round ends
   ROUND_START,  // the round starts
   SCOUT,  // a scout attack occurred (not yet needed afaik)
   SLAY,  // a unit is slain
   STRIKE,  // a unit strikes (anything)
   STUN,  // stunning a unit
   SUMMON,  // summoning a unit
   SUPPORT,  // supporting the unit on the right of the attacking unit
   TARGET,  // targeting a unit with a floating/skill
   UNIT_DAMAGE,  // any unit on the field takes damage
};

template <events::EventID event_type>
struct EventIDType {
   constexpr static events::EventID value = event_type;
};

constexpr const size_t n_events = static_cast< size_t >(EventID::UNIT_DAMAGE) + 1;

}  // namespace events

#endif  // LORAINE_EVENT_ID_H
