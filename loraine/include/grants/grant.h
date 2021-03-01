#ifndef LORAINE_GRANT_H
#define LORAINE_GRANT_H

#include <utility>

#include "cards/card.h"
#include "utils/types.h"
#include "utils/utils.h"

enum GrantType { STATS, MANA, KEYWORD, EFFECT };



class Grant {
   friend class GrantModifier;
  public:
   [[nodiscard]] inline auto get_grant_type() const { return m_grant_type; }
   [[nodiscard]] inline auto get_bestowing_card() const { return m_bestowing_card; }
   [[nodiscard]] inline auto get_bestowing_card() { return m_bestowing_card; }
   [[nodiscard]] inline auto get_bestowed_card() const { return m_bestowed_card; }
   [[nodiscard]] inline auto get_bestowed_card() { return m_bestowed_card; }
   [[nodiscard]] inline auto is_permanent() const { return m_permanent; }
   [[nodiscard]] inline auto get_uuid() const { return m_uuid; }

   void undo() { _undo(); }
   virtual void apply() = 0;

   [[nodiscard]] std::string explain() const;
   virtual ~Grant() = default;

   Grant(
      GrantType grant_type,
      sptr< Card > bestowing_card,
      sptr< Card > bestowed_card,
      bool permanent);

   Grant(const Grant& grant)
       : m_grant_type(grant.get_grant_type()), m_permanent(grant.is_permanent()), m_uuid(utils::new_uuid())
   {
   }
   Grant(Grant&& grant) = default;
   Grant& operator=(Grant&& rhs) = delete;
   Grant& operator=(const Grant& rhs) = delete;
   bool operator==(const Grant& rhs) { return m_uuid == rhs.get_uuid(); }
   bool operator!=(const Grant& rhs) { return m_uuid != rhs.get_uuid(); }

  private:
   GrantType m_grant_type;
   sptr< Card > m_bestowed_card;
   sptr< Card > m_bestowing_card;
   bool m_permanent;
   UUID m_uuid;

   virtual void _undo() = 0;
   [[nodiscard]] virtual std::string _explain() const { return ""; };
};

class StatsGrant: public Grant {
   friend class GrantModifier;
  public:
   StatsGrant(
      const sptr< Card >& bestowing_card,
      const sptr< Card >& bestowed_card,
      bool permanent,
      long int power,
      long health);

   void apply() override;

   [[nodiscard]] inline auto get_power_change() const { return m_power_change; }
   [[nodiscard]] inline auto get_health_change() const { return m_health_change; }

  private:
   long int m_power_change;
   // health can only be granted. Otherwise the common is meant to take damage,
   // which is not a StatsGrant!
   long m_health_change;

   void _undo() override;
};

class ManaGrant: public Grant {
   friend class GrantModifier;
  public:
   ManaGrant(
      const sptr< Card >& bestowing_card,
      const sptr< Card >& bestowed_card,
      bool permanent,
      long int mana_change);

   void apply() override
   {
      get_bestowed_card()->add_mana_cost(m_mana_change, is_permanent());
   }
   [[nodiscard]] inline auto get_mana_change() const { return m_mana_change; }

  private:
   const long int m_mana_change;

   void _undo() override { get_bestowed_card()->add_mana_cost(-m_mana_change, is_permanent()); }
};

class KeywordGrant: public Grant {
   friend class GrantModifier;
  public:
   KeywordGrant(
      const sptr< Card >& bestowing_card,
      const sptr< Card >& bestowed_card,
      bool permanent,
      enum Keyword kword);

   void apply() override
   {
      if(auto bestowed_card = get_bestowed_card(); not bestowed_card->has_keyword(m_keyword)) {
         bestowed_card->add_keyword(m_keyword);
      }
   }
   void set_keyword(Keyword kword) { m_keyword = kword; }
   [[nodiscard]] inline auto get_keyword() const { return m_keyword; }

  private:
   enum Keyword m_keyword;

   void _undo() override{};
};

class EffectGrant: public Grant {
   friend class GrantModifier;
  public:
   EffectGrant(
      const sptr< Card >& bestowing_card,
      const sptr< Card >& bestowed_card,
      bool permanent,
      sptr<EffectBase> effect,
      events::EventLabel event_type);

   void _undo() override
   {
      if(get_bestowed_card()->has_effect(m_event_type, *m_effect)) {
         get_bestowed_card()->remove_effect(m_event_type, *m_effect);
      }
   }
   void apply() override
   {
      get_bestowed_card()->add_effect(m_event_type, m_effect);
   }

   void set_event_type(events::EventLabel e_type) { m_event_type = e_type; }
   void set_effect(sptr<EffectBase> effect) { m_effect = std::move(effect); }

   [[nodiscard]] inline auto get_effect() const { return m_effect; }
   [[nodiscard]] inline auto get_event_type() const { return m_event_type; }

  private:
   sptr<EffectBase> m_effect;
   events::EventLabel m_event_type;
};

#endif  // LORAINE_GRANT_H
