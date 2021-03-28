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
 * Effectively an Enum class with a wrapper around it to provide some class bound methods
 */
struct Status {
  private:
   enum class Value {
      TIE = 0,
      ONGOING,
      BLUE_WINS_NEXUS,
      BLUE_WINS_DRAW,
      RED_WINS_NEXUS,
      RED_WINS_DRAW,
   };
   explicit Status(Value v) : value(v) {}

  public:
   static const Status TIE;
   static const Status ONGOING;
   static const Status BLUE_WINS_NEXUS;
   static const Status BLUE_WINS_DRAW;
   static const Status RED_WINS_NEXUS;
   static const Status RED_WINS_DRAW;

   constexpr static const size_t n_status = static_cast< size_t >(Value::RED_WINS_DRAW) + 1;

   inline explicit operator int() const { return static_cast< int >(value); }
   inline explicit operator size_t() const { return static_cast< size_t >(value); }

   static inline Status win(Team team, bool nexus)
   {
      if(team == Team::BLUE) {
         return nexus ? Status::BLUE_WINS_NEXUS : Status::BLUE_WINS_DRAW;
      }
      return nexus ? Status::RED_WINS_NEXUS : Status::RED_WINS_DRAW;
   }

   bool operator==(const Status& other) const { return value == other.value; }
   bool operator!=(const Status& other) const { return value != other.value; }

   Value value;
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
