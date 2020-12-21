#ifndef LORAINE_GRANT_H
#define LORAINE_GRANT_H

#include <utility>

#include "cards/card.h"
#include "types.h"
#include "uuid_gen.h"

enum GrantType { STATS, MANA, KEYWORD, EFFECT };

class Grant {
  public:
   void set_is_permanent(bool permanent) { m_permanent = permanent; }
   [[nodiscard]] inline auto get_grant_type() const { return m_grant_type; }
   [[nodiscard]] inline auto get_bestowing_card() const { return m_bestowing_card; }
   [[nodiscard]] inline auto is_permanent() const { return m_permanent; }
   [[nodiscard]] inline auto get_uuid() const { return m_uuid; }

   virtual void apply(const sptr< Card >& card_to_bestow) = 0;
   void undo(const sptr< Card >& bestowed_card)
   {
      auto card_grants = bestowed_card->get_all_grants();
      if(std::find_if(
            card_grants.begin(),
            card_grants.end(),
            [&](const sptr< Grant >& grant) { return *grant == *this; })
         != card_grants.end()) {
         _undo(bestowed_card);
      }
   };

   virtual ~Grant() = default;

   Grant(GrantType grant_type, sptr< Card > bestowing_card, bool permanent)
       : m_grant_type(grant_type),
         m_bestowing_card(std::move(bestowing_card)),
         m_permanent(permanent),
         m_uuid(new_uuid())
   {
   }

   Grant(const Grant& grant)
       : m_grant_type(grant.get_grant_type()), m_permanent(grant.is_permanent()), m_uuid(new_uuid())
   {
   }
   Grant(Grant&& grant) = default;
   Grant& operator=(Grant&& rhs) = delete;
   Grant& operator=(const Grant& rhs) = delete;
   bool operator==(const Grant& rhs) { return m_uuid == rhs.get_uuid(); }
   bool operator!=(const Grant& rhs) { return m_uuid != rhs.get_uuid(); }

  protected:
  private:
   GrantType m_grant_type;
   sptr< Card > m_bestowing_card;
   bool m_permanent;
   UUID m_uuid;

   virtual void _undo(const sptr< Card >& bestowed_card) = 0;
};

class StatsGrant: public Grant {
  public:
   StatsGrant(const sptr< Card >& bestowing_card, bool permanent, long int power, long health)
       : Grant(GrantType::STATS, bestowing_card, permanent),
         m_power_change(power),
         m_health_change(health)
   {
   }
   [[nodiscard]] inline auto get_power_change() const { return m_power_change; }
   [[nodiscard]] inline auto get_health_change() const { return m_health_change; }

   void apply(const sptr< Card >& card_to_bestow) override
   {
      bool permanent = is_permanent();
      auto unit_to_bestow = to_unit(card_to_bestow);
      unit_to_bestow->add_power(m_power_change, permanent);
      unit_to_bestow->add_health(m_health_change, permanent);
   }

  private:
   long int m_power_change;
   // health can only be granted. Otherwise the unit is meant to take damage,
   // which is not a StatsGrant!
   long m_health_change;

   void _undo(const sptr< Card >& bestowed_card) override
   {
      // we need to worry only about permanent effect, as temporary ones are
      // automatically removed at the end of a round
      auto unit = to_unit(bestowed_card);
      if(this->is_permanent()) {
         unit->add_power(-m_power_change, true);
         unit->add_health(-m_health_change, true);
      } else {
         unit->add_power(-m_power_change, false);
         unit->heal(m_health_change);
         unit->add_health(-m_health_change, false);
      }
   }
};

class ManaGrant: public Grant {
  public:
   ManaGrant(const sptr< Card >& bestowing_card, bool permanent, long int mana_change)
       : Grant(GrantType::MANA, bestowing_card, permanent), m_mana_change(mana_change)
   {
   }

   [[nodiscard]] inline auto get_mana_change() const { return m_mana_change; }

   void apply(const sptr< Card >& card_to_bestow) override
   {
      card_to_bestow->add_mana_cost(m_mana_change, is_permanent());
   }

  private:
   const long int m_mana_change;

   void _undo(const sptr< Card >& bestowed_card) override
   {
      bestowed_card->add_mana_cost(-m_mana_change, is_permanent());
   }
};

class KeywordGrant: public Grant {
  public:
   KeywordGrant(const sptr< Card >& bestowing_card, bool permanent, enum Keyword kword)
       : Grant(GrantType::KEYWORD, bestowing_card, permanent), m_keyword(kword)
   {
   }

   void set_keyword(Keyword kword) { m_keyword = kword; }
   [[nodiscard]] inline auto get_keyword() const { return m_keyword; }

   void apply(const sptr< Card >& card_to_bestow) override
   {
      if(not card_to_bestow->has_keyword(m_keyword)) {
         card_to_bestow->add_keyword(m_keyword);
      }
   }

  private:
   enum Keyword m_keyword;

   void _undo(const sptr< Card >& bestowed_card) override{};
};

class EffectGrant: public Grant {
  public:
   EffectGrant(
      const sptr< Card >& bestowing_card,
      bool permanent,
      Effect effect,
      events::EventType event_type)
       : Grant(GrantType::EFFECT, bestowing_card, permanent),
         m_effect(std::move(effect)),
         m_event_type(event_type)
   {
   }

   void _undo(const sptr< Card >& bestowed_card) override
   {
      if(bestowed_card->has_effect(m_event_type, m_effect)) {
         bestowed_card->remove_effect(m_event_type, m_effect);
      }
   }
   void apply(const sptr< Card >& card_to_bestow) override
   {
      card_to_bestow->add_effect(m_event_type, m_effect);
   }

   void set_event_type(events::EventType e_type) { m_event_type = e_type; }
   void set_effect(Effect effect) { m_effect = std::move(effect); }

   [[nodiscard]] inline auto get_effect() const { return m_effect; }
   [[nodiscard]] inline auto get_event_type() const { return m_event_type; }

  private:
   Effect m_effect;
   events::EventType m_event_type;
};


#endif  // LORAINE_GRANT_H
