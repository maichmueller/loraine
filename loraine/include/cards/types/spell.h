
#ifndef LORAINE_SPELL_H
#define LORAINE_SPELL_H

#include "cardbase.h"

class Spell: public Card {
   [[nodiscard]] bool _check_play_condition(const State& state) const override;
   [[nodiscard]] virtual bool _check_cast_condition(const Game& game) const;

  public:
   // use base class constructors
   using Card::Card;

   [[nodiscard]] bool is_spell() const override { return true; }
   [[nodiscard]] virtual bool check_cast_condition(const Game& game) const;
};

inline sptr< Spell > to_spell(const sptr< Card >& card)
{
   return std::dynamic_pointer_cast< Spell >(card);
}

#endif  // LORAINE_SPELL_H
