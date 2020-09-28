#ifndef LORAINE_GRANTS_H
#define LORAINE_GRANTS_H

#include <utility>

#include "card.h"
#include "uuid_gen.h"

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
   virtual void undo() = 0;
   [[nodiscard]] virtual sptr< Grant > copy_on(
      const sptr< Card >& card) const = 0;

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

   void undo() override
   {
      // we need to worry only about permanent effect, as temporary ones are
      // automatically removed at the end of a round
      auto unit = std::dynamic_pointer_cast< Unit >(this->get_bestowed_card());
      if(this->is_permanent()) {
         unit->change_power(-m_power_change, true);
         unit->change_health(-m_health_change, true);
      } else {
         unit->change_power(-m_power_change, false);
         unit->heal(m_power_change);
         unit->change_health(-m_power_change, false);
      }
   }

   [[nodiscard]] inline auto get_power_change() const { return m_power_change; }

   [[nodiscard]] inline auto get_health_change() const
   {
      return m_health_change;
   }

   [[nodiscard]] sptr< Grant > copy_on(const sptr< Card >& card) const override
   {
      return std::make_shared< StatsGrant >(
         get_name_of_cause(),
         to_unit(card),
         is_permanent(),
         get_power_change(),
         get_health_change());
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

   void undo() override
   {
      this->get_bestowed_card()->reduce_mana_cost(-m_mana_change);
   }

   [[nodiscard]] inline auto get_mana_change() const { return m_mana_change; }

   [[nodiscard]] sptr< Grant > copy_on(const sptr< Card >& card) const override
   {
      return std::make_shared< ManaGrant >(
         get_name_of_cause(), card, is_permanent(), get_mana_change());
   }

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

   void undo() override
   {
      if(! m_superfluous) {
         this->get_bestowed_card()->remove_keyword(m_keyword);
      }
   }

   [[nodiscard]] inline auto get_keyword() const { return m_keyword; }

   [[nodiscard]] sptr< Grant > copy_on(const sptr< Card >& card) const override
   {
      return std::make_shared< KeywordGrant >(
         get_name_of_cause(), card, is_permanent(), get_keyword());
   }

  private:
   const enum Keyword m_keyword;
   const bool m_superfluous;
};

class EffectGrant: public Grant {
   static std::optional< std::vector< EffectContainer > > _init_backup(
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
      return {};
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
         m_event_type(event_type),
         m_effect_backup(_init_backup(card_to_bestow, event_type))
   {
      card_to_bestow->add_effect(event_type, effect);
   }

   void undo() override
   {
      if(m_effect_backup.has_value()) {
         get_bestowed_card()->set_effect_vec(
            m_event_type, m_effect_backup.value());
      } else {
         get_bestowed_card()->remove_effect(m_event_type, m_effect);
      }
   }

   [[nodiscard]] inline auto get_effect() const { return m_effect; }
   [[nodiscard]] inline auto get_event_type() const { return m_event_type; }

   [[nodiscard]] sptr< Grant > copy_on(const sptr< Card >& card) const override
   {
      return std::make_shared< EffectGrant >(
         get_name_of_cause(),
         card,
         is_permanent(),
         get_effect(),
         get_event_type());
   }

  private:
   const EffectContainer m_effect;
   const events::EventType m_event_type;
   const std::optional< std::vector< EffectContainer > > m_effect_backup;
};

#endif  // LORAINE_GRANTS_H
