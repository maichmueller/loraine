
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
};
struct played {
};
struct nightfallen {
};
struct daybroken {
};
template < events::EventID event_id >
struct subscriber {
   constexpr static events::EventID id = event_id;
};
template < Keyword kw >
struct keyword {
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

struct alive {
};

}  // namespace tag

#endif  // LORAINE_TAGS_HPP
