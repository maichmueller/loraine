

#include "grants/grant.h"

Grant::Grant(
   GrantType grant_type, sptr< Card > bestowing_card, sptr< Card > bestowed_card, bool permanent)
    : m_grant_type(grant_type),
      m_bestowing_card(std::move(bestowing_card)),
      m_bestowed_card(std::move(bestowed_card)),
      m_permanent(permanent),
      m_uuid(new_uuid())
{
}
std::string Grant::explain() const
{
   std::stringstream ss;
   ss << m_bestowing_card->get_const_attrs().name;
   if(std::string sub_explanation = _explain(); not sub_explanation.empty()) {
      ss << "\n" << sub_explanation;
   }
   return ss.str();
}
StatsGrant::StatsGrant(
   const sptr< Card >& bestowing_card,
   const sptr< Card >& bestowed_card,
   bool permanent,
   long power,
   long health)
    : Grant(GrantType::STATS, bestowing_card, bestowed_card, permanent),
      m_power_change(power),
      m_health_change(health)
{
}
void StatsGrant::_undo()
{
   // we need to worry only about permanent effect, as temporary ones are
   // automatically removed at the end of a round
   auto unit = to_unit(get_bestowed_card());
   if(this->is_permanent()) {
      unit->add_power(-m_power_change, true);
      unit->add_health(-m_health_change, true);
   } else {
      unit->add_power(-m_power_change, false);
      unit->heal(m_health_change);
      unit->add_health(-m_health_change, false);
   }
}
void StatsGrant::apply() {

   bool permanent = is_permanent();
   auto unit_to_bestow = to_unit(get_bestowed_card());
   unit_to_bestow->add_power(m_power_change, permanent);
   unit_to_bestow->add_health(m_health_change, permanent);
}

ManaGrant::ManaGrant(
   const sptr< Card >& bestowing_card,
   const sptr< Card >& bestowed_card,
   bool permanent,
   long mana_change)
    : Grant(GrantType::MANA, bestowing_card, bestowed_card, permanent), m_mana_change(mana_change)
{
}
KeywordGrant::KeywordGrant(
   const sptr< Card >& bestowing_card,
   const sptr< Card >& bestowed_card,
   bool permanent,
   enum Keyword kword)
    : Grant(GrantType::KEYWORD, bestowing_card, bestowed_card, permanent), m_keyword(kword)
{
}
EffectGrant::EffectGrant(
   const sptr< Card >& bestowing_card,
   const sptr< Card >& bestowed_card,
   bool permanent,
   Effect effect,
   events::EventType event_type)
    : Grant(GrantType::EFFECT, bestowing_card, bestowed_card, permanent),
      m_effect(std::move(effect)),
      m_event_type(event_type)
{
}
