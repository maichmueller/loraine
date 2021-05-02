
#ifndef LORAINE_TAGS_HPP
#define LORAINE_TAGS_HPP

#include "loraine/events/event_id.h"

namespace tag {

struct card {
};
struct spell {
};
struct unit {
};
struct landmark {
};
struct trap {
};
struct nexus {
};
struct played {
};
struct nightfallen {
};
struct daybroken {
};
template < events::EventID event_id >
struct subscriber {
   constexpr static auto value = event_id;
};
template < Keyword kw >
struct keyword {
   constexpr static auto value = kw;
};
struct death {
};
template <Team::Value t>
struct team {
   constexpr static auto value = t;
};

// player tags

/// tag to allow finding the entities that mark the players
struct player {
};
/// the player that started the first turn
struct starting_player {
};
struct daybreak {
};
struct nightfall {
};
struct scout {
};
/// the player holds the attack token (i.e. is allowed to initiate a fight)
struct attack {
};
/// the player is the current attacker
struct attacker {
};
struct plunder {
};
struct enlightenment {
};
struct reputation {
};
struct has_played {
};
struct pass {
};
struct deep {
};

}  // namespace tag

#endif  // LORAINE_TAGS_HPP
