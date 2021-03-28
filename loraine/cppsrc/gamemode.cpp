
#include "core/gamemode.h"

#include <algorithms.h>

#include <utility>

#include "core/gamedefs.h"
#include "grants/grant.h"
#include "utils/utils.h"
#include "core/logic.h"

void GameMode::run_game()
{
   while(true) {
      if(auto terminality = m_state.logic()->step(); terminality != Status::ONGOING) {
         return;
      }
   }
}
