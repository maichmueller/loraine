
#ifndef LORAINE_SPELL_H
#define LORAINE_SPELL_H

#include "cardbase.h"

class Spell: public Cloneable<Spell, inherit_constructors<Card>> {
   [[nodiscard]] bool _check_play_condition(const State& state) const override;
   [[nodiscard]] virtual bool _check_cast_condition(const GameMode& game) const;

  public:
   // use base class constructors
   using Cloneable::Cloneable;

   [[nodiscard]] virtual bool check_cast_condition(const GameMode& game) const;
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
