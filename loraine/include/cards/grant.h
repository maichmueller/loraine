#ifndef LORAINE_GRANT_H
#define LORAINE_GRANT_H

#include <utility>

#include "cards/card.h"
#include "uuid_gen.h"
#include "types.h"

enum GrantType { Stats, Mana, Keyword, Effect };

class Grant {
  public:
   [[nodiscard]] inline auto get_grant_type() const { return m_grant_type; }
   [[nodiscard]] inline auto get_bestowed_card() const { return m_bestowed_card; }
   [[nodiscard]] inline auto get_bestowing_card() const { return m_bestowing_card; }
   [[nodiscard]] inline auto is_permanent() const { return m_permanent; }
   [[nodiscard]] inline auto get_uuid() const { return m_uuid; }

   virtual void undo() = 0;

   [[nodiscard]] inline sptr< Grant > copy_on(const sptr< Card >& card) const
   {
      return _copy_on(card);
   }

   virtual ~Grant() = default;

   Grant(
      GrantType grant_type,
      sptr< Card > bestowing_card,
      sptr< Card > card_to_bestow,
      bool permanent)
       : m_grant_type(grant_type),
         m_bestowing_card(std::move(bestowing_card)),
         m_bestowed_card(std::move(card_to_bestow)),
         m_permanent(permanent),
         m_uuid(new_uuid())
   {
   }
   Grant(const Grant& grant)
       : m_grant_type(grant.get_grant_type()),
         m_bestowing_card(grant.get_bestowed_card()),
         m_bestowed_card(grant.get_bestowed_card()),
         m_permanent(grant.is_permanent()),
         m_uuid(new_uuid())
   {
   }
   Grant(Grant&& grant) noexcept
       : m_grant_type(grant.m_grant_type),
         m_bestowing_card(grant.m_bestowing_card),
         m_bestowed_card(grant.m_bestowed_card),
         m_permanent(grant.m_permanent),
         m_uuid(grant.m_uuid)
   {
   }
   Grant& operator=(Grant&& rhs) = delete;
   Grant& operator=(const Grant& rhs) = delete;
   bool operator==(const Grant& rhs) { return m_uuid == rhs.get_uuid(); }

   bool operator!=(const Grant& rhs) { return m_uuid != rhs.get_uuid(); }

  protected:
  private:
   const GrantType m_grant_type;
   const sptr< Card > m_bestowing_card;
   const sptr< Card > m_bestowed_card;
   const bool m_permanent;
   const UUID m_uuid;

   [[nodiscard]] virtual sptr< Grant > _copy_on(const sptr< Card >& card) const = 0;
};

class StatsGrant: public Grant {
  public:
   StatsGrant(
      const sptr< Card >& bestowing_card,
      const sptr< Unit >& card_to_bestow,
      bool permanent,
      long int power,
      size_t health)
       : Grant(
          GrantType::Stats,
          bestowing_card,
          card_to_bestow,
          permanent),
         m_power_change(power),
         m_health_change(health)
   {
      card_to_bestow->add_power(power, permanent);
      card_to_bestow->add_health(power, permanent);
   }

   void undo() override
   {
      // we need to worry only about permanent effect, as temporary ones are
      // automatically removed at the end of a round
      auto unit = to_unit(this->get_bestowed_card());
      if(this->is_permanent()) {
         unit->add_power(-m_power_change, true);
         unit->add_health(-static_cast< long >(m_health_change), true);
      } else {
         unit->add_power(-m_power_change, false);
         unit->heal(m_health_change);
         unit->add_health(-static_cast< long >(m_health_change), false);
      }
   }

   [[nodiscard]] inline auto get_power_change() const { return m_power_change; }
   [[nodiscard]] inline auto get_health_change() const { return m_health_change; }

  private:
   const long int m_power_change;
   // health can only be granted. Otherwise the unit is meant to take damage,
   // which is not a StatsGrant!
   const size_t m_health_change;

   [[nodiscard]] sptr< Grant > _copy_on(const sptr< Card >& card) const override
   {
      return std::make_shared< StatsGrant >(
         get_bestowing_card(),
         to_unit(card),
         is_permanent(),
         get_power_change(),
         get_health_change());
   }
};

class ManaGrant: public Grant {
  public:
   ManaGrant(
      const sptr< Card >& bestowing_card,
      const sptr< Card >& card_to_bestow,
      bool permanent,
      long int mana_change)
       : Grant(GrantType::Mana, bestowing_card, card_to_bestow, permanent),
         m_mana_change(mana_change)
   {
      card_to_bestow->add_mana_cost(mana_change, permanent);
   }

   void undo() override
   {
      this->get_bestowed_card()->add_mana_cost(-m_mana_change, is_permanent());
   }

   [[nodiscard]] inline auto get_mana_change() const { return m_mana_change; }

  private:
   const long int m_mana_change;

   [[nodiscard]] sptr< Grant > _copy_on(const sptr< Card >& card) const override
   {
      return std::make_shared< ManaGrant >(
         get_bestowing_card(), card, is_permanent(), get_mana_change());
   }
};

class KeywordGrant: public Grant {
  public:
   KeywordGrant(
      const sptr< Card >& bestowing_card,
      const sptr< Card >& card_to_bestow,
      bool permanent,
      enum Keyword kword)
       : Grant(GrantType::Keyword, bestowing_card, card_to_bestow, permanent),
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

  private:
   const enum Keyword m_keyword;
   const bool m_superfluous;

   [[nodiscard]] sptr< Grant > _copy_on(const sptr< Card >& card) const override
   {
      return std::make_shared< KeywordGrant >(
         get_bestowing_card(), card, is_permanent(), get_keyword());
   }
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
      const sptr< Card >& bestowing_card,
      const sptr< Card >& card_to_bestow,
      bool permanent,
      EffectContainer effect,
      events::EventType event_type)
       : Grant(GrantType::Effect, bestowing_card, card_to_bestow, permanent),
         m_effect(std::move(effect)),
         m_event_type(event_type),
         m_effect_backup(_init_backup(card_to_bestow, event_type))
   {
      card_to_bestow->add_effect(event_type, effect);
   }

   void undo() override
   {
      if(m_effect_backup.has_value()) {
         get_bestowed_card()->set_effect_vec(m_event_type, m_effect_backup.value());
      } else {
         get_bestowed_card()->remove_effect(m_event_type, m_effect);
      }
   }

   [[nodiscard]] inline auto get_effect() const { return m_effect; }
   [[nodiscard]] inline auto get_event_type() const { return m_event_type; }

  private:
   const EffectContainer m_effect;
   const events::EventType m_event_type;
   const std::optional< std::vector< EffectContainer > > m_effect_backup;

   [[nodiscard]] sptr< Grant > _copy_on(const sptr< Card >& card) const override
   {
      return std::make_shared< EffectGrant >(
         get_bestowing_card(), card, is_permanent(), get_effect(), get_event_type());
   }
};

class GrantFactory {
  public:
   template < GrantType grant_type, typename... Params >
   inline sptr< Grant > grant(Params&&... params)
   {
      if constexpr(grant_type == Stats) {
         return _grant_stats(std::forward< Params... >(params...));
      } else if constexpr(grant_type == Mana) {
         return _grant_mana(std::forward< Params... >(params...));
      } else if constexpr(grant_type == Keyword) {
         return _grant_keyword(std::forward< Params... >(params...));
      } else if constexpr(grant_type == Effect) {
         return _grant_effect(std::forward< Params... >(params...));
      }
   }

   inline void fix_permanent(bool permanent) { m_permanent = permanent; }
   inline void fix_card_filter(std::function< bool(const sptr< Card >&) > card_filter)
   {
      m_card_filter = card_filter;
   }
   inline void fix_mana_change(long mana_change) { m_mana_change = mana_change; }
   inline void fix_power_change(long power_change) { m_power_change = power_change; }
   inline void fix_health_change(long health_change) { m_health_change = health_change; }
   inline void fix_effect(const EffectContainer& effect) { m_effect = effect; }
   inline void fix_event_type(events::EventType event_type) { m_event_type = event_type; }
   inline void fix_keyword(enum Keyword keyword) { m_keyword = keyword; }

   inline void reset_permanent() { m_permanent.reset(); }
   inline void reset_card_filter() { m_card_filter.reset(); }
   inline void reset_mana_change() { m_mana_change.reset(); }
   inline void reset_health_change() { m_power_change.reset(); }
   inline void reset_power_change() { m_health_change.reset(); }
   inline void reset_effect() { m_effect.reset(); }
   inline void reset_event_type() { m_event_type.reset(); }
   inline void reset_keyword() { m_keyword.reset(); }

  private:
   std::optional< bool > m_permanent;
   std::optional< std::function< bool(const sptr< Card >&) > > m_card_filter;
   std::optional< long int > m_power_change;
   std::optional< size_t > m_health_change;
   std::optional< long int > m_mana_change;
   std::optional< enum Keyword > m_keyword;
   std::optional< EffectContainer > m_effect;
   std::optional< events::EventType > m_event_type;

   inline sptr< Grant > _grant_stats(
      const sptr< Card >& bestowing_card,
      const sptr< Unit >& card_to_bestow,
      bool permanent,
      long int power,
      size_t health)
   {
      if(m_permanent.has_value()) {
         permanent = m_permanent.value();
      }
      if(m_power_change.has_value()) {
         power = m_power_change.value();
      }
      if(m_health_change.has_value()) {
         health = m_health_change.value();
      }
      return std::make_shared< StatsGrant >(
         bestowing_card, card_to_bestow, permanent, power, health);
   }
   inline sptr< Grant > _grant_mana(
      const sptr< Card >& bestowing_card,
      const sptr< Unit >& card_to_bestow,
      bool permanent,
      long int mana_change)
   {
      if(m_permanent.has_value()) {
         permanent = m_permanent.value();
      }
      if(m_mana_change.has_value()) {
         mana_change = m_mana_change.value();
      }
      return std::make_shared< ManaGrant >(bestowing_card, card_to_bestow, permanent, mana_change);
   }
   inline sptr< Grant > _grant_keyword(
      const sptr< Card >& bestowing_card,
      const sptr< Unit >& card_to_bestow,
      bool permanent,
      enum Keyword kword)
   {
      if(m_permanent.has_value()) {
         permanent = m_permanent.value();
      }
      if(m_keyword.has_value()) {
         kword = m_keyword.value();
      }
      return std::make_shared< KeywordGrant >(bestowing_card, card_to_bestow, permanent, kword);
   }
   inline sptr< Grant > _grant_effect(
      const sptr< Card >& bestowing_card,
      const sptr< Unit >& card_to_bestow,
      bool permanent,
      EffectContainer effect,
      events::EventType event_type)
   {
      if(m_permanent.has_value()) {
         permanent = m_permanent.value();
      }
      if(m_effect.has_value()) {
         effect = m_effect.value();
      }
      if(m_event_type.has_value()) {
         event_type = m_event_type.value();
      }
      return std::make_shared< EffectGrant >(
         bestowing_card, card_to_bestow, permanent, std::move(effect), event_type);
   }

};

#endif  // LORAINE_GRANT_H
