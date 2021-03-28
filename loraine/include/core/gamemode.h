
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

class CardToken;


class GameMode {
  public:

   void run_game();

   GameMode(const Config& cfg, SymArr<Deck > decks, SymArr<sptr<Controller>> controllers)
      : m_state(cfg, decks, controllers)
   {
   }
//   GameMode(const Config& cfg, SymArr<std::vector<CardToken> > decks, SymArr<sptr<Controller>> controllers)
//      : m_state(cfg, decks, controllers)
//   {
//   }

   auto state() { return m_state; }
   [[nodiscard]] auto state() const { return m_state; }

  private:
   State m_state;
};

#endif  // LORAINE_GAMEMODE_H
