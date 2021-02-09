
#include "target.h"

#include "cards/card.h"

Target::Target(const sptr< Card >& card) : player(card->get_mutable_attrs().owner) {}
