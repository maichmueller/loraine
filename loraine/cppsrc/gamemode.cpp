
#include "core/gamemode.h"

#include <utils/algorithms.h>

#include <utility>

#include "core/gamedefs.h"
#include "core/logic.h"
#include "grants/grant.h"
#include "utils/utils.h"

void GameMode::run_game()
{
   while(true) {
      if(auto terminality = m_state.logic()->step(); terminality != Status::ONGOING) {
         return;
      }
   }
}
