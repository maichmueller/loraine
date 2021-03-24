
#ifndef LORAINE_RECORD_H
#define LORAINE_RECORD_H

#include <utility>

#include "action.h"
#include "gamedefs.h"

struct Record {
  public:
   Record() = default;
   Record(const Record&) = default;
   Record(Record&&) = default;
   Record& operator=(const Record&) = default;
   Record& operator=(Record&&) = default;

   virtual ~Record() = default;
};
struct ActionRecord: public Record {
   ActionRecord(actions::Action action, Team team) : team(team), action(std::move(action)) {}

  private:
   Team team;
   actions::Action action;
};

template < typename EventClass >
struct EventRecord: public Record {
   using args_tuple = typename EventClass::SignatureTuple;
   explicit EventRecord(args_tuple&& t) : args(std::move(t)) {}

  private:
   args_tuple args;
};

#endif  // LORAINE_RECORD_H
