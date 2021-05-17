
#ifndef LORAINE_TARGETER_HPP
#define LORAINE_TARGETER_HPP

#include "loraine/core/gamedefs.h"


class Targeter {
  public:
   enum Mode {
      MANUAL = 0,
      AUTOMATIC
   };

   using FilterFunc = std::function< bool(entt::registry&, entt::entity)>;
   Mode mode;
   Zone target_zone;
   FilterFunc filter;
   uint n_targets;
};


#endif  // LORAINE_TARGETER_HPP
