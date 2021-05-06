
#include "loraine/core/systems/system.hpp"
#include "loraine/core/gamestate.h"

void System::context(GameState& state) {
   m_gamestate = &state;
   m_registry = &state.registry();
}
