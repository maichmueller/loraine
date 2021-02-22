
#include "target.h"

#include "cards/card.h"

Target::Target(const sptr< Card >& card) : team(card->mutables().owner) {}
