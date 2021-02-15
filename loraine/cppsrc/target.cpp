
#include "target.h"

#include "cards/card.h"

Target::Target(const sptr< Card >& card) : player(card->mutables().owner) {}
