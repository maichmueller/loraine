#ifndef LORAINE_GAMEDEFS_H
#define LORAINE_GAMEDEFS_H

#include <cstddef>
#include <cstdint>

enum Team { BLUE = 0, RED };

inline Team opponent(Team team)
{
   return Team(1 - static_cast< uint16_t >(team));
}

constexpr const size_t n_teams = static_cast< size_t >(Team::RED) + 1;


/**
 * Effectively an Enum class with a wrapper around it to provide some class utils
 */
struct Status {
   enum Value {
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

   template<typename IntType, typename = std::enable_if_t<std::is_integral_v<IntType>>>
   inline explicit operator IntType() const { return static_cast< IntType >(value); }

   static inline Status win(Team team, bool nexus)
   {
      if(team == Team::BLUE) {
         return nexus ? Status::BLUE_WINS_NEXUS : Status::BLUE_WINS_DRAW;
      }
      return nexus ? Status::RED_WINS_NEXUS : Status::RED_WINS_DRAW;
   }

   inline auto is_checked() const { return m_checked;}
   inline void mark_checked() { m_checked = true;}
   inline void uncheck() { m_checked = false;}

   bool operator==(const Status& other) const { return value == other.value; }
   bool operator!=(const Status& other) const { return value != other.value; }
   bool operator==(const Value& other) const { return value == other; }
   bool operator!=(const Value& other) const { return value != other; }

  public:
   Value value = ONGOING;
  private:
   bool m_checked = false;
};

enum class Location {
   BATTLEFIELD = 0,
   CAMP,
   BOARD,  // Board = Camp + Battlefield
   DECK,
   EVERYWHERE,
   GRAVEYARD,
   TOSSED,
   HAND,
   SPELLSTACK
};

constexpr const size_t n_locations = static_cast< size_t >(Location::SPELLSTACK) + 1;

#endif  // LORAINE_GAMEDEFS_H
