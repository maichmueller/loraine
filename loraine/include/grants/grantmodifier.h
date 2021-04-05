

#ifndef LORAINE_GRANTMODIFIER_H
#define LORAINE_GRANTMODIFIER_H

#include <utility>

#include "grant.h"

/*
 * Abstract Base Class for modifying grants after their play_event_triggers, but before execution time.
 *
 * */
class GrantModifier {
  public:
   explicit GrantModifier(std::function< bool(Grant&) > filter = [](Grant& /*g*/) { return true; })
       : m_filter(std::move(filter)), m_uuid(utils::new_uuid()){};

   void operator()(Grant& g)
   {
      if(m_filter(g)) {
         _modify(g);
      }
   };

   [[nodiscard]] auto get_uuid() const { return m_uuid; }

   bool operator==(const GrantModifier& other) const { return m_uuid == other.get_uuid(); }

  protected:
   // modifier functions that children can access to change aspects of the grant.
   static void _set_permanence(Grant& g, bool permanence_value)
   {
      g.m_permanent = permanence_value;
   }
   static void _set_bestowing_card(Grant& g, sptr< Card > bestowing_card)
   {
      g.m_bestowing_card = std::move(bestowing_card);
   }
   static void _set_bestowed_card(Grant& g, sptr< Card > bestowing_card)
   {
      g.m_bestowing_card = std::move(bestowing_card);
   }
   static void _set_keyword(KeywordGrant& kg, Keyword kword) { kg.m_keyword = kword; }

  private:
   std::function< bool(Grant&) > m_filter;
   UUID m_uuid;

   virtual void _modify(Grant& g) = 0;
};

/*
 * making all grants, subject to the filter, permanent
 */
class PermanentGrantModifier: public GrantModifier {
  public:
   using GrantModifier::GrantModifier;
   inline void _modify(Grant& grant) override { _set_permanence(grant, true); }

  private:
};

// TODO: Move this eventually to the respective Card implementation area
bool out_of_the_way_filter(Grant& g);

#endif  // LORAINE_GRANTMODIFIER_H
