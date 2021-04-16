
#include "loraine/effects/effect.h"

bool IEffect::operator==(const IEffect& effect) const
{
   return m_uuid == effect.uuid();
}

bool IEffect::operator!=(const IEffect& effect) const
{
   return not (*this == effect);
}
IEffect::IEffectComponent(const IEffect& effect)
    : Targeting(effect),
      m_effect_label(effect.m_style),
      m_consumed(effect.m_consumed),
      m_assoc_card(effect.m_assoc_card),
      m_uuid(utils::new_uuid())
{
}
