#ifndef LORAINE_GRANTS_H
#define LORAINE_GRANTS_H

#include <utility>
#include "uuid_gen.h"
#include "card.h"

enum GrantType { Stats, Mana, Keyword, Effect };

class Grant {
  public:
   [[nodiscard]] inline auto get_grant_type() const { return m_grant_type; }

   [[nodiscard]] inline auto get_bestowed_card() const
   {
      return m_bestowed_card;
   }
   [[nodiscard]] inline auto get_name_of_cause() const
   {
      return m_name_of_cause;
   }

   [[nodiscard]] inline auto get_uuid() const { return m_uuid; }
   [[nodiscard]] inline auto is_permanent() const { return m_permanent; }
   virtual void remove() = 0;

   explicit Grant(
      GrantType grant_type,
      const char* const name_of_cause,
      sptr< Card > card_to_bestow,
      bool permanent)
       : m_grant_type(grant_type),
         m_name_of_cause(name_of_cause),
         m_bestowed_card(std::move(card_to_bestow)),
         m_permanent(permanent),
         m_uuid(new_uuid())
   {
   }

   Grant(const Grant& grant)
       : m_grant_type(grant.get_grant_type()),
         m_name_of_cause(grant.get_name_of_cause()),
         m_bestowed_card(grant.get_bestowed_card()),
         m_permanent(grant.is_permanent()),
         m_uuid(new_uuid())
   {
   }

  protected:
   void _deregister()
   {
      m_bestowed_card->get_grants_map().erase(this->get_uuid());
   };

  private:
   const GrantType m_grant_type;
   const char* const m_name_of_cause;
   const sptr< Card > m_bestowed_card;
   const bool m_permanent;
   const UUID m_uuid;
};

class StatsGrant: public Grant {
  public:
   StatsGrant(
      const char* const name_of_cause,
      const sptr< Unit >& card_to_bestow,
      bool permanent,
      long int power,
      size_t health)
       : Grant(GrantType::Stats, name_of_cause, card_to_bestow, permanent),
         m_power_change(power),
         m_health_change(health)
   {
      card_to_bestow->change_power(power, permanent);
      card_to_bestow->change_health(power, permanent);
   }

   void remove() override
   {
      // we need to worry only about permanent effect, as temporary ones are
      // automatically removed at the end of a round
      auto unit = std::dynamic_pointer_cast< Unit >(this->get_bestowed_card());
      if(bool permanent = this->is_permanent(); permanent) {
         unit->change_power(-m_power_change, permanent);
         unit->change_health(-m_health_change, permanent);
      } else {
         unit->change_power(-m_power_change, permanent);
         unit->heal(m_power_change);
         unit->change_health(-m_power_change, permanent);
      }
      _deregister();
   }

   [[nodiscard]] inline auto get_power_change() const { return m_power_change; }
   [[nodiscard]] inline auto get_health_change() const
   {
      return m_health_change;
   }

  private:
   const long int m_power_change;
   // health can only be granted. Otherwise the unit is meant to take damage,
   // which is not a StatsGrant!
   const size_t m_health_change;
};

class ManaGrant: public Grant {
  public:
   ManaGrant(
      const char* const name_of_cause,
      const sptr< Card >& card_to_bestow,
      bool permanent,
      long int mana_change)
       : Grant(GrantType::Mana, name_of_cause, card_to_bestow, permanent),
         m_mana_change(mana_change)
   {
      card_to_bestow->reduce_mana_cost(mana_change);
   }

   void remove() override
   {
      this->get_bestowed_card()->reduce_mana_cost(-m_mana_change);
      _deregister();
   }

   [[nodiscard]] inline auto get_mana_change() const { return m_mana_change; }

  private:
   const long int m_mana_change;
};

class KeywordGrant: public Grant {
  public:
   KeywordGrant(
      const char* const name_of_cause,
      const sptr< Card >& card_to_bestow,
      bool permanent,
      enum Keyword kword)
       : Grant(GrantType::Mana, name_of_cause, card_to_bestow, permanent),
         m_keyword(kword),
         m_superfluous(card_to_bestow->has_keyword(kword))
   {
      if(! m_superfluous) {
         card_to_bestow->add_keyword(kword);
      }
   }

   void remove() override
   {
      if(! m_superfluous) {
         this->get_bestowed_card()->remove_keyword(m_keyword);
      }
      _deregister();
   }

   [[nodiscard]] inline auto get_keyword() const { return m_keyword; }

  private:
   const enum Keyword m_keyword;
   const bool m_superfluous;
};

class EffectGrant: public Grant {
   static std::optional< EffectContainer > _init_backup(
      const sptr< Card >& card, events::EventType event_type)
   {
      auto effects_map = card->get_effects_map();
      if(std::find_if(
            effects_map.begin(),
            effects_map.end(),
            [&](const auto& val) { return val.first == event_type; })
         != effects_map.end()) {
         return effects_map.at(event_type);
      }
      return std::optional< EffectContainer >();
   }

  public:
   EffectGrant(
      const char* const name_of_cause,
      const sptr< Card >& card_to_bestow,
      bool permanent,
      EffectContainer effect,
      events::EventType event_type)
       : Grant(GrantType::Effect, name_of_cause, card_to_bestow, permanent),
         m_effect(std::move(effect)),
         m_effect_backup(_init_backup(card_to_bestow, event_type)),
         m_event_type(event_type)
   {
      card_to_bestow->set_effect(event_type, effect);
   }

   void remove() override
   {
      if(m_effect_backup.has_value()) {
         get_bestowed_card()->set_effect(m_event_type, m_effect_backup.value());
      } else {
         get_bestowed_card()->remove_effect(m_event_type);
      }
      _deregister();
   }

   [[nodiscard]] inline auto get_effect() const { return m_effect; }

  private:
   const EffectContainer m_effect;
   const std::optional< EffectContainer > m_effect_backup;
   const events::EventType m_event_type;
};

#endif  // LORAINE_GRANTS_H
