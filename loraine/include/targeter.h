
#ifndef LORAINE_TARGETER_H
#define LORAINE_TARGETER_H

#include <utility>

#include "core/gamedefs.h"
#include "core/targetable.h"
#include "filters/filter.h"
#include "utils/types.h"

class State;

enum class TargetingTeam { ENEMY = 0, OWN, ANY };
enum class TargetingMode {
   MANUAL = 0,
   AUTOMATIC,
};

class TargeterBase {
  public:
   explicit TargeterBase(TargetingMode mode) : m_mode(mode), m_filter() {}
   explicit TargeterBase(Filter filter)
       : m_mode(TargetingMode::AUTOMATIC), m_filter(std::move(filter))
   {
   }
   TargeterBase(TargetingMode mode, Filter filter) : m_mode(mode), m_filter(std::move(filter)) {}

   virtual std::vector< sptr< Targetable > > operator()(const State& state, Team team) = 0;

   bool is_automatic() { return m_mode == TargetingMode::AUTOMATIC; }
   explicit operator bool() { return bool(m_filter); }

   void filter_targets(std::vector< sptr< Targetable > >& targets)
   {
      targets.erase(
         std::remove_if(
            targets.begin(),
            targets.end(),
            [&](const sptr< Targetable >& target) { return not m_filter(*target); }),
         targets.end());
   }

  private:
   TargetingMode m_mode;
   Filter m_filter{};
};

template < TargetingMode mode, TargetingTeam affiliation, Location range >
class Targeter: public TargeterBase {
};

class NoneTargeter:
    public Targeter< TargetingMode::AUTOMATIC, TargetingTeam::ANY, Location::BOARD > {
};

template < TargetingTeam affiliation, Location range >
class AutomaticTargeter: public Targeter< TargetingMode::AUTOMATIC, affiliation, range > {
};

#endif  // LORAINE_TARGETER_H
