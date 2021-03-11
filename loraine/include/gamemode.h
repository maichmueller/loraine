
#ifndef LORAINE_GAMEMODE_H
#define LORAINE_GAMEMODE_H

#include <algorithms.h>

#include <optional>
#include <utility>

#include "cards/card.h"
#include "controller.h"
#include "core/config.h"
#include "core/gamedefs.h"
#include "core/state.h"
#include "filters/filter.h"
#include "grants/grant.h"
#include "grants/grantfactory.h"
#include "utils/types.h"

class GameMode {
  public:

   bool run_game();

   GameMode(Config cfg, SymArr<const char*> decks)
       : m_state(std::move(std::make_shared<State>()))
   {
   }

   auto state() { return m_state; }
   [[nodiscard]] auto state() const { return m_state; }

  private:
   sptr< State > m_state;
};

#endif  // LORAINE_GAMEMODE_H
