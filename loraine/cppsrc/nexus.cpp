
#include "engine/nexus.h"

bool Nexus::has_effect(events::EventLabel e_type, const EffectBase& effect) const
{
   auto found_effects = m_effects.find(e_type);
   bool found = found_effects != m_effects.end();
   if(found) {
      const auto& effects = found_effects->second;
      return std::find_if(
                effects.begin(),
                effects.end(),
                [&](const sptr< EffectBase >& e) { return *e == effect; })
             != effects.end();
   }
   return false;
}
