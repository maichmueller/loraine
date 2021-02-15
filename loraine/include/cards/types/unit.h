
#ifndef LORAINE_UNIT_H
#define LORAINE_UNIT_H

#include <utility>

#include "cardbase.h"
#include "fieldcard.h"

class Unit: public FieldCard {
  public:
   struct ConstUnitData {
      // the fixed reference damage the unit deals.
      const size_t power_ref;
      // the fixed reference health of the unit
      const size_t health_ref;
   };

   struct MutableUnitData {
      // the permanent base power of the unit (can be moved by e.g. effects)
      size_t power_base;
      // the permanent base health of the unit (can be moved by e.g. effects)
      size_t health_base;
      // the current change to the unit power (temporary buffs/nerfs).
      long int power_delta = 0;
      // the current change to the unit health (temporary buffs/nerfs).
      long int health_delta = 0;
      // the damage the unit has taken
      size_t damage = 0;
      // whether the unit is dead
      bool alive = true;
      // whether the unit is stunned (cant block or attack)
      bool stunned = false;
      // the function to kill the unit with (e.g. needs to be changed by Unyielding Spirit)
      std::function< void(Unit&) > kill_func = &_default_kill;
   };

   [[nodiscard]] inline auto& unit_mutable_attrs() const { return m_unit_mutables; }
   [[nodiscard]] inline auto& unit_mutable_attrs() { return m_unit_mutables; }
   [[nodiscard]] inline auto& unit_const_attrs() const { return m_unit_immutables; }
   [[nodiscard]] inline auto& unit_const_attrs() { return m_unit_immutables; }
   inline void kill() { m_unit_mutables.kill_func(*this); }

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
   inline void take_damage(size_t amount) { m_unit_mutables.damage += amount; }

   inline void heal(size_t amount)
   {
      m_unit_mutables.damage -= std::min(m_unit_mutables.damage, amount);
   }

   [[nodiscard]] bool is_unit() const override { return true; }

   template < typename... Args >
   Unit(ConstUnitData const_unit_data, MutableUnitData mutable_unit_data, Args&&... args)
       : Card(std::forward< Args >(args)...),
         m_unit_mutables(std::move(mutable_unit_data)),
         m_unit_immutables(std::move(const_unit_data))
   {
   }
   ~Unit() override = default;
   Unit(const Unit& card) = default;
   Unit& operator=(const Unit& unit) = delete;
   Unit(Unit&&) = delete;
   Unit& operator=(Unit&&) = delete;

  private:
   MutableUnitData m_unit_mutables;
   ConstUnitData m_unit_immutables;

   [[nodiscard]] bool _check_play_condition(const State& state) const override;
   static void _default_kill(Unit& unit) { unit.unit_mutable_attrs().alive = false; }
};

inline sptr< Unit > to_unit(const sptr< Card >& card)
{
   return std::dynamic_pointer_cast< Unit >(card);
}

#endif  // LORAINE_UNIT_H
