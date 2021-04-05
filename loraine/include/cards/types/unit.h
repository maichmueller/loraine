
#ifndef LORAINE_UNIT_H
#define LORAINE_UNIT_H

#include <utility>

#include "cardbase.h"
#include "fieldcard.h"

class Unit: public Cloneable< Unit, inherit_constructors< FieldCard > > {
  public:
   struct ConstUnitState {
      // the fixed reference damage the unit deals.
      const size_t power_ref;
      // the fixed reference health of the unit
      const size_t health_ref;
   };

   struct MutableUnitState {
      // the permanent base power of the unit (can be moved by e.g. m_effects)
      size_t power_base;
      // the permanent base health of the unit (can be moved by e.g. m_effects)
      size_t health_base;
      // the current change to the unit power (temporary buffs/nerfs).
      long int power_delta = 0;
      // the current change to the unit health (temporary buffs/nerfs).
      long int health_delta = 0;
      // the damage the unit has taken
      size_t damage = 0;
      // whether the unit is dead
      bool alive = true;
      // the function to damage the unit with (e.g. needs to be flexible for Armored Elephant)
      std::vector< std::function< long(Card&, long) > > dmg_modifiers{};
      // the function to kill the unit with (e.g. needs to be flexible for Unyielding Spirit)
      std::function< void(const sptr< Card >& /*cause*/) > kill_func;
   };

   [[nodiscard]] inline auto& unit_mutables() const { return m_unit_mutables; }
   [[nodiscard]] inline auto& unit_mutables() { return m_unit_mutables; }
   [[nodiscard]] inline auto& unit_immutables() const { return m_unit_immutables; }
   [[nodiscard]] inline auto& unit_immutables() { return m_unit_immutables; }

   void power(size_t power, bool as_delta = true);
   [[nodiscard]] long power_raw() const;
   [[nodiscard]] inline size_t power() const { return std::max(0L, power_raw()); }
   void add_power(long int amount, bool permanent);
   void health(size_t health);
   [[nodiscard]] long health_raw() const;
   [[nodiscard]] inline size_t health() const { return std::max(long(0), health_raw()); }
   void add_health(long int amount, bool permanent);
   long take_damage(const sptr< Card >& damaging_card, long amount);
   void kill(const sptr< Card >& cause);
   void heal(size_t amount);

   Unit(
      ConstState const_state,
      MutableState mutable_state,
      ConstUnitState const_unit_state,
      MutableUnitState mutable_unit_state)
       : Cloneable(const_state, std::move(mutable_state)),
         m_unit_mutables(std::move(mutable_unit_state)),
         m_unit_immutables(std::move(const_unit_state))
   {
   }
   ~Unit() override = default;
   Unit(const Unit& unit) = default;
   Unit& operator=(const Unit& unit) = delete;
   Unit(Unit&&) = delete;
   Unit& operator=(Unit&&) = delete;

  private:
   const ConstUnitState m_unit_immutables;
   MutableUnitState m_unit_mutables;
};

inline sptr< Unit > to_unit(const sptr< Card >& card)
{
   return std::dynamic_pointer_cast< Unit >(card);
}


#endif  // LORAINE_UNIT_H
