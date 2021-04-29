
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
template < Zone zone >
struct location {
   constexpr static auto value = zone;
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

struct daybreak {
};
struct nightfall {
};
struct scout {
};
struct attack {
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
