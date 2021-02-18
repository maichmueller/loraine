
#ifndef LORAINE_FILTER_H
#define LORAINE_FILTER_H

#include <utility>

#include "cards/card_defs.h"
#include "engine/rulesets.h"
#include "target.h"
#include "utils/types.h"

class Card;

class Filter {
  public:
   bool operator()(const Target& target) const { return m_filter(target); }
   Filter() : m_filter([](const Target& /*unused*/) { return true; }) {}
   explicit Filter(std::function< bool(const Target& /*target*/) > filter)
       : m_filter(std::move(filter))
   {
   }

  private:
   std::function< bool(const Target& /*target*/) > m_filter;
};

#endif  // LORAINE_FILTER_H
