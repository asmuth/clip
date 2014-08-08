/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _FNORDMETRIC_QUERY_LIMITCLAUSE_H
#define _FNORDMETRIC_QUERY_LIMITCLAUSE_H
#include <stdlib.h>
#include <string>
#include <vector>
#include <assert.h>
#include "token.h"
#include "astnode.h"
#include "queryplannode.h"

namespace fnordmetric {
namespace query {

class LimitClause : public QueryPlanNode {
public:

  LimitClause(int limit, int offset, QueryPlanNode* child) :
      limit_(limit),
      offset_(offset),
      child_(child),
      counter_(0) {
    child->setTarget(this);
  }

  void execute() override {
    child_->execute();
  }

  size_t getNumCols() const override {
    return child_->getNumCols();
  }

  bool nextRow(SValue* row, int row_len) override {
    if (counter_++ < offset_) {
      return true;
    }

    if (counter_ > (offset_ + limit_)) {
      return false;
    }

    emitRow(row, row_len);
    return true;
  }

  const std::vector<std::string>& getColumns() const override {
    return child_->getColumns();
  }

protected:
  size_t limit_;
  size_t offset_;
  QueryPlanNode* child_;
  size_t counter_;
};

}
}
#endif
