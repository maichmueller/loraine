
#include "cards/effect.h"

bool EffectBase::operator==(const EffectBase& effect) const
{
   return m_uuid == effect.uuid();
}

bool EffectBase::operator!=(const EffectBase& effect) const
{
   return not (*this == effect);
}
EffectBase::EffectBase(const EffectBase& effect)
    : m_effect_type(effect.m_effect_type),
      m_consumed(effect.m_consumed),
      m_assoc_card(effect.m_assoc_card),
      m_uuid(utils::new_uuid()),
      m_targeter(effect.m_targeter),
      m_targets(effect.m_targets)
{
}
