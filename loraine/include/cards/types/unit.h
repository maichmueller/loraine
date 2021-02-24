
#ifndef LORAINE_UNIT_H
#define LORAINE_UNIT_H

#include <utility>

#include "cardbase.h"
#include "fieldcard.h"

class Unit: public FieldCard {
  public:
   struct ConstUnitState {
      // the fixed reference damage the common deals.
      const size_t power_ref;
      // the fixed reference health of the common
      const size_t health_ref;
   };

   struct MutableUnitState {
      // the permanent base power of the common (can be moved by e.g. m_effects)
      size_t power_base;
      // the permanent base health of the common (can be moved by e.g. m_effects)
      size_t health_base;
      // the current change to the common power (temporary buffs/nerfs).
      long int power_delta = 0;
      // the current change to the common health (temporary buffs/nerfs).
      long int health_delta = 0;
      // the damage the common has taken
      size_t damage = 0;
      // whether the common is dead
      bool alive = true;
      // the function to damage the common with (e.g. needs to be flexible for Armored Elephant)
      std::function< long(Unit&, long) > take_damage_func = &_default_damage;
      // the function to kill the common with (e.g. needs to be flexible for Unyielding Spirit)
      std::function< void(Unit&) > kill_func = &_default_kill;
   };

   [[nodiscard]] inline auto& unit_mutables() const { return m_unit_mutables; }
   [[nodiscard]] inline auto& unit_mutables() { return m_unit_mutables; }
   [[nodiscard]] inline auto& unit_immutables() const { return m_unit_immutables; }
   [[nodiscard]] inline auto& unit_immutables() { return m_unit_immutables; }
   void power(size_t power, bool as_delta = true);

   [[nodiscard]] inline auto power_raw() const
   {
      return m_unit_mutables.power_base + static_cast< size_t >(m_unit_mutables.power_delta);
   }
   [[nodiscard]] inline auto power() const { return std::max(size_t(0), power_raw()); }
   void add_power(long int amount, bool permanent);
   void health(size_t health);

   [[nodiscard]] inline auto health_raw() const
   {
      long health_base = static_cast< long int >(m_unit_mutables.health_base);
      long damage = static_cast< long >(m_unit_mutables.damage);
      return health_base + m_unit_mutables.health_delta - damage;
   }
   [[nodiscard]] inline auto health() const { return std::max(long(0), health_raw()); }
   void add_health(long int amount, bool permanent);
   inline auto take_damage(size_t amount)
   {
      return m_unit_mutables.take_damage_func(*this, amount);
   }
   inline void kill() { m_unit_mutables.kill_func(*this); }

   inline void heal(size_t amount)
   {
      m_unit_mutables.damage -= std::min(m_unit_mutables.damage, amount);
   }

   [[nodiscard]] bool is_unit() const override { return true; }

   Unit(
      ConstState const_state,
      MutableState mutable_state,
      ConstUnitState const_unit_state,
      MutableUnitState mutable_unit_state)
       : FieldCard(const_state, std::move(mutable_state)),
         m_unit_mutables(std::move(mutable_unit_state)),
         m_unit_immutables(std::move(const_unit_state))
   {
   }
   ~Unit() override = default;
   Unit(const Unit& card) = default;
   Unit& operator=(const Unit& unit) = delete;
   Unit(Unit&&) = delete;
   Unit& operator=(Unit&&) = delete;

  private:
   const ConstUnitState m_unit_immutables;
   MutableUnitState m_unit_mutables;

   [[nodiscard]] bool _check_play_condition(const State& state) const override;
   static void _default_kill(Unit& unit) { unit.unit_mutables().alive = false; }
   static long _default_damage(Unit& unit, long amount)
   {
      unit.unit_mutables().damage += amount;
      return amount;
   }
};

inline sptr< Unit > to_unit(const sptr< Card >& card)
{
   return std::dynamic_pointer_cast< Unit >(card);
}

#endif  // LORAINE_UNIT_H
