
#include "engine/nexus.h"
bool Nexus::has_effect(events::EventType e_type, const EffectBase& effect) const
{
   auto found_effects = m_effects.find(e_type);
   bool found = found_effects != m_effects.end();
   if(found) {
      const auto& effects = found_effects->second;
      return std::find(effects.begin(), effects.end(), effect) != effects.end();
   }
   return false;
}
