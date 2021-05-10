#ifndef LORAINE_GAMEDEFS_H
#define LORAINE_GAMEDEFS_H

#include <cstddef>
#include <cstdint>

#include "loraine/utils/utils.h"

enum struct Team { BLUE = 0, RED };

Team opponent(Team t) { return Team(1 - as_int(t));}

constexpr const size_t n_teams = static_cast< size_t >(Team::RED) + 1;

/**
 * Effectively an Enum class with a wrapper around it to provide some class utils
 */
struct Status {
   enum Value : uint8_t {
      TIE = 0,
      ONGOING,
      BLUE_WINS_NEXUS,
      BLUE_WINS_DRAW,
      RED_WINS_NEXUS,
      RED_WINS_DRAW,
   };
   Status() = default;
   Status(Value v) : value(v), m_checked(v == ONGOING ? false : true) {}

   constexpr static const size_t n_status = static_cast< size_t >(RED_WINS_DRAW) + 1;

   template < typename IntType, typename = std::enable_if_t< std::is_integral_v< IntType > > >
   inline explicit operator IntType() const
   {
      return static_cast< IntType >(value);
   }

   static inline Status win(Team team, bool nexus)
   {
      if(team == Team::BLUE) {
         return nexus ? Status::BLUE_WINS_NEXUS : Status::BLUE_WINS_DRAW;
      }
      return nexus ? Status::RED_WINS_NEXUS : Status::RED_WINS_DRAW;
   }

   inline auto is_checked() const { return m_checked; }
   inline void mark_checked() { m_checked = true; }
   inline void uncheck() { m_checked = false; }

   inline bool operator==(const Status& other) const { return value == other.value; }
   inline bool operator!=(const Status& other) const { return value != other.value; }
   inline bool operator==(const Value& other) const { return value == other; }
   inline bool operator!=(const Value& other) const { return value != other; }

  public:
   Value value = ONGOING;

  private:
   bool m_checked = false;
};

enum class Zone {
   TOSSED = 0,
   GRAVEYARD,
   SPELLSTACK,
   HAND,
   BATTLEFIELD,
   CAMP,
   DECK,
};

constexpr const size_t n_zone = static_cast< size_t >(Zone::DECK) + 1;

enum struct Rarity { NONE = 0, CHAMPION, COMMON, EPIC, RARE };

constexpr const size_t n_rarities = static_cast< size_t >(Rarity::RARE) + 1;

enum struct Group {
   NONE = 0,
   ASCENDED,
   CELESTIAL,
   DRAGON,
   ELITE,
   ELNUK,
   PORO,
   SEA_MONSTER,
   SPIDER,
   TECH,
   TREASURE,
   YETI
};

constexpr const size_t n_groups = static_cast< size_t >(Group::YETI) + 1;

enum struct CardType { SPELL, UNIT, LANDMARK, TRAP };

constexpr const size_t n_cardtypes = static_cast< size_t >(CardType::LANDMARK) + 1;

enum struct CardSuperType { NONE, CHAMPION, SKILL };

constexpr const size_t n_cardsupertypes = static_cast< size_t >(CardSuperType::SKILL) + 1;

enum struct Region {
   BILGEWATER = 0,
   DEMACIA,
   FRELJORD,
   IONIA,
   NOXUS,
   PILTOVER_ZAUN,
   SHADOW_ISLES,
   TARGON,
};

constexpr const size_t n_regions = static_cast< size_t >(Region::TARGON) + 1;


enum class Keyword {
   ALLEGIANCE = 0,  // define the starting value, necessary for indexing.
   ATTACK,
   ATTUNE,
   BARRIER,
   BEHOLD,
   BURST,
   CANT_BLOCK,
   CAPTURE,
   CHALLENGER,
   DAYBREAK,
   DEEP,
   DOUBLE_ATTACK,
   DRAIN,
   ELUSIVE,
   ENLIGHTENED,
   EPHEMERAL,
   FAST,
   FEARSOME,
   FOCUS,
   FLEETING,
   FROSTBITE,
   FURY,
   NEXUS_STRIKE,
   NIGHTFALL,
   OBLITERATE,
   IMMOBILE,
   INVOKE,
   LAST_BREATH,
   LIFESTEAL,
   OVERWHELM,
   PLUNDER,
   QUICK_ATTACK,
   RECALL,
   REGENERATION,
   SCOUT,
   SKILL,
   SLOW,
   SPELLSHIELD,
   STRIKE,
   STRONGEST,
   STUN,
   SUPPORT,
   TOSS,
   TOUGH,
   TRAP,
   VULNERABLE,
   WEAKEST,
};

constexpr const size_t n_keywords = static_cast< size_t >(Keyword::WEAKEST);

#endif  // LORAINE_GAMEDEFS_H
