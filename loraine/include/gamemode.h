
#ifndef LORAINE_GAMEMODE_H
#define LORAINE_GAMEMODE_H

#include <algorithms.h>

#include <optional>
#include <utility>

#include "cards/card.h"
#include "controller.h"
#include "engine/config.h"
#include "engine/gamedefs.h"
#include "engine/state.h"
#include "filters/filter.h"
#include "grants/grant.h"
#include "grants/grantfactory.h"
#include "utils/types.h"

class GameMode {
  public:

   bool run_game();

   GameMode(sptr< State > state)
       : m_state(std::move(state))
   {
   }

   auto state() { return m_state; }
   [[nodiscard]] auto state() const { return m_state; }

  private:
   sptr< State > m_state;
};

#endif  // LORAINE_GAMEMODE_H
