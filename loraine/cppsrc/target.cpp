
#include "target.h"

#include "cards/card_types/all_card_types.h"

Target::Target(const sptr< Card >& card) : player(card->get_mutable_attrs().owner) {}
