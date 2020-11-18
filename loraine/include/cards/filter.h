
#ifndef LORAINE_FILTER_H
#define LORAINE_FILTER_H

#include <utility>

#include "card_defs.h"
#include "rulesets.h"
#include "types.h"

class Card;

class Filter {
  public:
   bool operator()(const sptr< Card >& card) const { return m_filter(card); }
   Filter() : m_filter([](const sptr< Card >& /*unused*/) { return true; }) {}
   explicit Filter(std::function< bool(const sptr< Card >& card) > filter)
       : m_filter(std::move(filter))
   {
   }

  private:
   std::function< bool(const sptr< Card >& card) > m_filter;
};

#endif  // LORAINE_FILTER_H
