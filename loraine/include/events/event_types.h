

#ifndef LORAINE_EVENT_TYPES_H
#define LORAINE_EVENT_TYPES_H

namespace events {

enum class EventType {
   NONE = 0,
   ATTACK,  // declaring attacker
   BEHOLD,  // the player has an 8+ mana unit in hand or in play
   CAPTURE,  // A unit has captured another unit
   CAST,  // casting spells/skills (activation events, not committing events)
   BLOCK,  // declaring blockers
   DAYBREAK,  // the played card is the first to be played in this round
   DIE,  // a unit dies
   DISCARD,  // removing a card from hand (not playing)
   DRAW_CARD,  // draw a card (own deck or enemy deck)
   ENLIGHTENMENT,  // reaching 10 mana gems
   GAIN_MANAGEM,  // gaining an amount of mana (e.g. round-start)
   HEAL_UNIT,  // healing a unit for some amount
   LEVEL_UP,  // champion levels up
   NEXUS_STRIKE,  // from direct attack
   NIGHTFALL,  // the played card is not the first to be played in this round
   PLAY,  // summoning a unit from hand or playing a spell
   RECALL,  // a unit gets recalled from field back to the hand
   ROUND_END,  // the round ends
   ROUND_START,  // the round starts
   STRIKE,  // a unit strikes (anything)
   SCOUT,  // a scout attack occurred (not yet needed afaik)
   STUN,  // stunning a unit
   SUMMON,  // summoning a unit
   SUPPORT,  // supporting the unit on the right of the attacking unit
   TARGET,  // targeting a unit with a spell/skill
   UNIT_TAKE_DAMAGE,  // any unit on the field takes damage
   COUNTER
};

constexpr const static size_t n_events = static_cast<size_t>(EventType::COUNTER) + 1;
}

#endif  // LORAINE_EVENT_TYPES_H
