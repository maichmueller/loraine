
#include "cards/effect.h"
#include "cards/card.h"
#include "game.h"

Player EffectContainer::get_owner() const
{
   return m_assoc_card->get_owner();
}
void EffectContainer::operator()(Game& game, const events::AnyEvent& event)
{
   if(check_cast_condition(game)) {
      if(m_effect_type != Type::AOE) {
         if(has_targets()) {
            // in case something has changed in between playing and casting the targets need to be
            // refiltered
            m_targeter->filter_targets(m_targets);
         }
      } else {
         // AOE effects need to redo their targeting to adapt to any changes inbetween playing
         // and casting the effect
         Player player = get_owner();
         choose_targets(*game.get_state(), *(game.get_agent(player)), player);
      }
   }
   m_effect_func(game, event, *this);
}
bool EffectContainer::operator==(const EffectContainer& effect) const
{
   return m_is_null == effect.is_null() && get_owner() == effect.get_owner()
          && m_effect_type == effect.get_effect_type() && m_location == effect.get_location()
          && get_address(m_effect_func) == get_address(effect.get_effect_func())
          && get_address(m_cast_con_func) == get_address(effect.get_cast_condition_func());
}
bool EffectContainer::operator!=(const EffectContainer& effect) const
{
   return ! (*this == effect);
}
