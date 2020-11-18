
#ifndef LORAINE_TARGETER_H
#define LORAINE_TARGETER_H

#include <utility>

#include "cards/filter.h"
#include "target.h"

class Agent;
class State;

class BaseTargeter {
  public:
   enum class Affiliation { enemy = 0, own, any };
   enum class Type {
      manual = 0,
      automatic,
   };

   BaseTargeter(
      Type type, Affiliation affiliation, Location range, size_t n_targets, Filter filter = {})
       : m_affiliation(affiliation),
         m_type(type),
         m_range(range),
         m_nr_targets(n_targets),
         m_filter(std::move(filter))
   {
   }

   std::vector< Target > operator()(const State& state, Agent& agent, Player player)
   {
      return _target(state, agent, player);
   }

   void filter_targets(std::vector< Target >& targets)
   {
      targets.erase(
         std::remove_if(
            targets.begin(),
            targets.end(),
            [&](const Target& target) { return not m_filter(target.opt_card.value()); }),
         targets.end());
   }

   [[nodiscard]] auto get_type() const { return m_type; }
   [[nodiscard]] auto get_affiliation() const { return m_affiliation; }
   [[nodiscard]] auto get_range() const { return m_range; }
   [[nodiscard]] auto get_nr_targets() const { return m_nr_targets; }
   [[nodiscard]] auto get_filter() const { return m_filter; }

  private:
   Type m_type;
   Affiliation m_affiliation;
   Location m_range;
   size_t m_nr_targets;
   Filter m_filter;

   virtual std::vector< Target > _target(const State& state, Agent& agent, Player player) = 0;
};

class NoneTargeter: public BaseTargeter {
  public:
   NoneTargeter() : BaseTargeter(Type::automatic, Affiliation::any, Location::EVERYWHERE, 0) {}

  private:
   std::vector< Target > _target(
      const State& /*state*/, Agent& /*agent*/, Player /*player*/) override
   {
      return std::vector< Target >();
   }
};

class EnemyCampManTargeter: public BaseTargeter {
   EnemyCampManTargeter(Affiliation affiliation, long n_targets, Filter filter = {})
       : BaseTargeter(Type::manual, affiliation, Location::CAMP, n_targets, std::move(filter))
   {
   }

  private:
   std::vector< Target > _target(const State& state, Agent& agent, Player acting_player) override;
};

#endif  // LORAINE_TARGETER_H
