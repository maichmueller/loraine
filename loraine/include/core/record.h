
#ifndef LORAINE_RECORD_H
#define LORAINE_RECORD_H

#include <utility>

#include "action.h"
#include "gamedefs.h"

struct Record {
  public:
   virtual ~Record() = default;
};
struct ActionRecord: public Record {
   ActionRecord(sptr< Action > action, Team team) : team(team), action(std::move(action)) {}

  private:
   Team team;
   sptr< Action > action;
};

template < typename EventClass >
struct EventRecord: public Record {
   using args_tuple = typename EventClass::SignatureTuple;
   explicit EventRecord(args_tuple&& t) : args(std::move(t)) {}

  private:
   args_tuple args;
};

#endif  // LORAINE_RECORD_H
