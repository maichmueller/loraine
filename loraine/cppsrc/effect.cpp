
#include "cards/effect.h"



bool EffectBase::operator==(const EffectBase& effect) const
{
   return m_uuid == effect.uuid();
}

bool EffectBase::operator!=(const EffectBase& effect) const
{
   return not (*this == effect);
}