
#ifndef LORAINE_TARGETER_H
#define LORAINE_TARGETER_H

#include <utility>

#include "filters/filter.h"
#include "target.h"

class State;

enum class TargetAffiliation { enemy = 0, own, any };
enum class TargetMode {
   Manual = 0,
   Automatic,
};

class TargeterBase {
  public:
   TargeterBase(Filter filter = {}) : m_filter(std::move(filter)) {}

   virtual std::vector< Target > operator()(const State& state, Player player)
   {
      return _target(state, player);
   }

   void filter_targets(std::vector< Target >& targets)
   {
      targets.erase(
         std::remove_if(
            targets.begin(),
            targets.end(),
            [&](const Target& target) { return not m_filter(target); }),
         targets.end());
   }

   [[nodiscard]] auto filter() const { return m_filter; }

  private:
   Filter m_filter;

   virtual std::vector< Target > _target(const State& state, Player player) = 0;
};

template < TargetMode mode, TargetAffiliation affiliation, Location range >
class Targeter: public TargeterBase {
};

template <TargetAffiliation affiliation, Location range >
class AutomaticTargeter : public Targeter< TargetMode::Automatic, affiliation, range > {

};


class NoneTargeter: public TargeterBase {
  public:
//   NoneTargeter()
//       : TargeterBase(TargetMode::automatic, TargetAffiliation::any, Location::EVERYWHERE, 0)
//   {
//   }

  private:
   std::vector< Target > _target(const State& /*state*/, Player /*player*/) override
   {
      return std::vector< Target >();
   }
};

//class EnemyCampManTargeter: public TargeterBase {
//   EnemyCampManTargeter(TargetAffiliation affiliation, long n_targets, Filter filter = {})
//       : TargeterBase(TargetMode::manual, affiliation, Location::CAMP, n_targets, std::move(filter))
//   {
//   }
//
//  private:
//   std::vector< Target > _target(const State& state, Player acting_player) override;
//};

#endif  // LORAINE_TARGETER_H
