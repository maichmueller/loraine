
#ifndef LORAINE_FILTER_H
#define LORAINE_FILTER_H

#include <utility>

#include "card_defs.h"
#include "rulesets.h"
#include "types.h"
#include "target.h"

class Card;

class Filter {
  public:
   bool operator()(const sptr< BaseTarget >& target) const { return m_filter(target); }
   Filter() : m_filter([](const sptr< BaseTarget >& /*unused*/) { return true; }) {}
   explicit Filter(std::function< bool(const sptr< BaseTarget >& /*target*/) > filter)
       : m_filter(std::move(filter))
   {
   }

  private:
   std::function< bool(const sptr< BaseTarget >& /*target*/) > m_filter;
};

#endif  // LORAINE_FILTER_H
