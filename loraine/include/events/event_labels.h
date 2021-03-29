

#ifndef LORAINE_EVENT_LABELS_H
#define LORAINE_EVENT_LABELS_H

namespace events {

enum class EventLabel {
   ATTACK = 0,  // declaring attacker
   BEHOLD,  // the team has an 8+ mana common in hand or in play
   BLOCK, // a unit blocked an attacking unit
   CAPTURE,  // A common has captured another common
   CAST,  // casting spells/skills (activation events, not committing events)
   DAYBREAK,  // the played card is the first to be played in this round
   DIE,  // a common dies
   DISCARD,  // removing a card from hand (not playing)
   DRAW_CARD,  // draw a card (own deck or enemy deck)
   ENLIGHTENMENT,  // reaching 10 mana gems
   GAIN_MANAGEM,  // gaining an amount of mana (e.g. round-start)
   HEAL_UNIT,  // healing a common for some amount
   LEVEL_UP,  // champion levels up
   NEXUS_DAMAGE,  // from non-combat attack
   NEXUS_STRIKE,  // from combat attack
   NIGHTFALL,  // the played card is not the first to be played in this round
   PLAY,  // summoning a common from hand or playing a floating
   RECALL,  // a common gets recalled from field back to the hand
   ROUND_END,  // the round ends
   ROUND_START,  // the round starts
   STRIKE,  // a common strikes (anything)
   SCOUT,  // a scout attack occurred (not yet needed afaik)
   STUN,  // stunning a common
   SUMMON,  // summoning a common
   SUPPORT,  // supporting the common on the right of the attacking common
   TARGET,  // targeting a common with a floating/skill
   UNIT_DAMAGE,  // any common on the field takes damage
};

template <events::EventLabel event_type>
struct EventLabelType {
   events::EventLabel value = event_type;
};

constexpr const size_t n_events = static_cast< size_t >(EventLabel::UNIT_DAMAGE) + 1;

}  // namespace lor_events

#endif  // LORAINE_EVENT_LABELS_H
