
#include "cards/effect.h"

#include "cards/card.h"
#include "game.h"

Player Effect::get_owner() const
{
   return m_assoc_card->get_owner();
}
bool Effect::operator()(Game& game, const events::AnyEvent& event)
{
   bool castable = check_condition(game);
   if(castable) {
      _call(game, event);
   }
   return castable;
}
bool Effect::operator==(const Effect& effect) const
{
   return m_is_null == effect.is_null() && get_owner() == effect.get_owner()
          && m_effect_type == effect.get_effect_type() && m_location == effect.get_location()
          && get_address(m_effect_func) == get_address(effect.get_effect_func())
          && get_address(m_con_func) == get_address(effect.get_condition_func());
}
bool Effect::operator!=(const Effect& effect) const
{
   return ! (*this == effect);
}
void Effect::_call(Game& game, const events::AnyEvent& event)
{
   m_effect_func(game, event, *this);
}
void TargetEffect::_call(Game& game, const events::AnyEvent& event)
{
   if(has_targets()) {
      // in case something has changed in between playing and casting the targets need to be
      // refiltered
      m_targeter->filter_targets(m_targets);
   }
   //   } else {
   //      // AOE effects need to redo their targeting to adapt to any changes inbetween playing
   //      // and casting the effect
   //      Player player = get_owner();
   //      set_targets(std::move(find_available_targets(*game.get_state(), player)));
   //   }
   get_effect_func()(game, event, *this);
}