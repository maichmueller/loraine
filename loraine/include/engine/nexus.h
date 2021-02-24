
#ifndef LORAINE_NEXUS_H
#define LORAINE_NEXUS_H

#include <map>
#include <utility>

#include "cards/card_defs.h"
#include "cards/effect.h"
#include "events/event_listener.h"
#include "gamedefs.h"

class Nexus: public EventListener< Nexus > {
   Nexus(Team team, long health, EffectMap emap = {}, KeywordMap kwordmap = {})
       : m_team(team), m_health(health), m_keywords(kwordmap), m_effects(std::move(emap))
   {
   }

   [[nodiscard]] inline auto team() const { return m_team; }
   [[nodiscard]] inline auto health() const { return m_health; }
   [[nodiscard]] inline auto name() const { return m_name; }
   [[nodiscard]] inline auto effects() const { return m_effects; }
   [[nodiscard]] auto& effects(events::EventType etype) { return m_effects.at(etype); }
   [[nodiscard]] auto& effects(events::EventType etype) const { return m_effects.at(etype); }
   [[nodiscard]] inline bool has_effect(events::EventType e_type) const
   {
      return m_effects.find(e_type) != m_effects.end();
   }
   [[nodiscard]] bool has_effect(events::EventType e_type, const EffectBase& effect) const;

   inline void change_health(long health) { m_health += health; }

  private:
   Team m_team;
   long m_health;
   const char* m_name = "Nexus";
   KeywordMap m_keywords;
   EffectMap m_effects;
};

#endif  // LORAINE_NEXUS_H
