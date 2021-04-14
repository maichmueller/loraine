
#ifndef LORAINE_TARGETING_H
#define LORAINE_TARGETING_H

#include <utility>

#include "core/gamedefs.h"
#include "utils/types.h"

// marker class for something that can be targeted
class Targetable {
};

using Filter = std::function< bool(const Targetable& /*target*/) >;

class GameState;

enum class TargetingTeam { ENEMY = 0, OWN, ANY };
enum class TargetingMode {
   MANUAL = 0,
   AUTOMATIC,
};

class TargeterBase: public Cloneable< abstract_method< TargeterBase > > {
  public:
   explicit TargeterBase(TargetingMode mode) : m_mode(mode), m_filter() {}
   explicit TargeterBase(Filter filter)
       : m_mode(TargetingMode::AUTOMATIC), m_filter(std::move(filter))
   {
   }
   TargeterBase(TargetingMode mode, Filter filter) : m_mode(mode), m_filter(std::move(filter)) {}

   virtual std::vector< sptr< Targetable > > operator()(const GameState& state, Team team) = 0;

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
   const TargetingMode m_mode;
   Filter m_filter{};
};

template < TargetingMode mode, TargetingTeam affiliation, Zone range >
class Targeter:
    public Cloneable<
       abstract_method< Targeter< mode, affiliation, range > >,
       inherit_constructors< TargeterBase > > {
  private:
   using base = Cloneable<
      abstract_method< Targeter< mode, affiliation, range > >,
      inherit_constructors< TargeterBase > >;

  public:
   Targeter() : base(mode) {}
   explicit Targeter(const Filter& filter) : base(mode, filter) {}
};

template < TargetingTeam affiliation, Zone range >
class AutomaticTargeter:
    public Cloneable<
       abstract_method< AutomaticTargeter< affiliation, range > >,
       inherit_constructors< Targeter< TargetingMode::AUTOMATIC, affiliation, range > > > {
  private:
   using base = Cloneable<
      abstract_method< AutomaticTargeter< affiliation, range > >,
      inherit_constructors< Targeter< TargetingMode::AUTOMATIC, affiliation, range > > >;

  public:
   using base::base;
};

class NoneTargeter:
    public Cloneable<
       NoneTargeter,
       AutomaticTargeter< TargetingTeam::ANY, Zone::EVERYWHERE > > {
  private:
   using base = Cloneable<
      NoneTargeter,
      AutomaticTargeter< TargetingTeam::ANY, Zone::EVERYWHERE > >;

  public:
   NoneTargeter() : base() {}
   std::vector< sptr< Targetable > > operator()(const GameState& /*state*/, Team /*team*/) override
   {
      return {};
   };
};

class Targeting {
  public:
   Targeting(sptr< TargeterBase > targeter) : m_targeter(targeter) {}

   inline void targets(std::vector< sptr< Targetable > > targets)
   {
      m_targets = std::move(targets);
   }
   [[nodiscard]] bool has_targets() const { return not m_targets.empty(); }
   [[nodiscard]] auto& targets() { return m_targets; }

   [[nodiscard]] auto targets() const { return m_targets; }
   inline void reset_targets() { m_targets.clear(); }
   [[nodiscard]] auto targeter() const { return m_targeter; }

   [[nodiscard]] auto& targeter() { return m_targeter; }

  private:
   sptr< TargeterBase > m_targeter;
   std::vector< sptr< Targetable > > m_targets{};
};

#endif  // LORAINE_TARGETING_H
