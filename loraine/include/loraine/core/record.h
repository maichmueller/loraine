
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
   ActionRecord(sptr<actions::Action> action) : m_action(std::move(action)) {}

   [[nodiscard]] auto team() const { return m_action->team();}
   [[nodiscard]] auto action() const { return m_action;}

  private:
   sptr<actions::Action> m_action;
};

template < typename EventClass >
struct EventRecord: public Record {
   using args_tuple = typename EventClass::SignatureTuple;
   explicit EventRecord(args_tuple&& t) : args(std::move(t)) {}

  private:
   args_tuple args;
};

#endif  // LORAINE_RECORD_H
