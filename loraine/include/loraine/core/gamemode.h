
#ifndef LORAINE_GAMEMODE_H
#define LORAINE_GAMEMODE_H

#include <loraine/utils/algorithms.h>

#include <optional>
#include <utility>

#include "config.hpp"
#include "gamedefs.h"
#include "gamestate.h"
#include "loraine/core/components/controller.hpp"
#include "loraine/grants/grant.h"
#include "loraine/grants/grantfactory.h"
#include "loraine/utils/types.h"

class CardToken;


class GameMode {
  public:

   void run_game();

//   GameMode(const Config& cfg, SymArr<Deck > decks, SymArr<sptr<Controller>> controllers)
//      : m_state(cfg, decks, controllers)
//   {
//   }
//   GameMode(const Config& cfg, SymArr<std::vector<CardToken> > decks, SymArr<sptr<Controller>> controllers)
//      : m_handler(cfg, decks, controllers)
//   {
//   }

   auto state() { return m_state; }
   [[nodiscard]] auto state() const { return m_state; }

  private:
   GameState m_state;
};

#endif  // LORAINE_GAMEMODE_H
