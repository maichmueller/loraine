
#ifndef LORAINE_SPELL_H
#define LORAINE_SPELL_H

#include "cardbase.h"

class Spell: public Cloneable<Spell, inherit_constructors<Card>> {

  public:
   // use base class constructors
   using Cloneable::Cloneable;
};

class Skill : public Spell {
  public:
   // use base class constructors
   using Spell::Spell;

};

inline sptr< Spell > to_spell(const sptr< Card >& card)
{
   return std::dynamic_pointer_cast< Spell >(card);
}

inline sptr< Skill > to_skill(const sptr< Card >& card)
{
   return std::dynamic_pointer_cast< Skill >(card);
}

#endif  // LORAINE_SPELL_H
