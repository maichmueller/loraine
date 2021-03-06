
#ifndef LORAINE_NEXUS_H
#define LORAINE_NEXUS_H

#include <map>
#include <utility>

#include "cards/card_defs.h"
#include "core/targeting.h"
#include "effects/effect.h"
#include "events/event_listener.h"
#include "events/event_subscriber.h"
#include "events/lor_events/event_labels.h"
#include "gamedefs.h"

class Nexus: public EventListener< Nexus >, public Targetable {
  public:
   using EffectMap = std::map< events::EventLabel, std::vector< sptr< EffectBase > > >;

   Nexus(Team team, long health, EffectMap emap = {}, KeywordMap kwordmap = {})
       : m_team(team), m_health(health), m_keywords(kwordmap), m_effects(std::move(emap))
   {
   }

   [[nodiscard]] inline auto team() const { return m_team; }
   [[nodiscard]] inline auto health() const { return m_health; }
   [[nodiscard]] inline auto name() const { return m_name; }
   [[nodiscard]] inline auto effects() const { return m_effects; }
   [[nodiscard]] auto& effects(events::EventLabel etype) { return m_effects.at(etype); }
   [[nodiscard]] auto& effects(events::EventLabel etype) const { return m_effects.at(etype); }
   [[nodiscard]] inline bool has_effect(events::EventLabel e_type) const
   {
      return m_effects.find(e_type) != m_effects.end();
   }
   [[nodiscard]] bool has_effect(events::EventLabel e_type, const EffectBase& effect) const;
   [[nodiscard]] inline bool has_keyword(Keyword kword) const
   {
      return m_keywords.at(static_cast< unsigned long >(kword));
   }
   [[nodiscard]] inline bool has_any_keyword(std::initializer_list< Keyword > kwords) const
   {
      return std::any_of(kwords.begin(), kwords.end(), [&](const auto& kw) {
         return m_keywords.at(static_cast< unsigned long >(kw));
      });
   }

   inline void add_health(const sptr<Card>& card, long health)
   {
      for(auto& modifier : m_damage_modifiers) {
         modifier(*card, health);
      }
      m_health += health;
   }

  private:
   Team m_team;
   const std::string m_name = "Nexus";
   long m_health;
   KeywordMap m_keywords;
   EffectMap m_effects;
   std::vector< std::function< void(Card&, long) > > m_damage_modifiers{};

};

#endif  // LORAINE_NEXUS_H
