/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _FNORDMETRIC_SQL_QUERYPLANNODE_H
#define _FNORDMETRIC_SQL_QUERYPLANNODE_H
#include <stdlib.h>
#include <string>
#include <vector>
#include <assert.h>
#include "svalue.h"
#include "token.h"
#include "astnode.h"
#include "rowsink.h"

namespace fnordmetric {
namespace query {

class QueryPlanNode : public RowSink {
public:
  QueryPlanNode();
  virtual ~QueryPlanNode();

  virtual void execute() = 0;
  virtual size_t getNumCols() const = 0;
  virtual const std::vector<std::string>& getColumns() const = 0;
  int getColumnIndex(const std::string& column_name) const;

  void setTarget(RowSink* target);
  void finish() override;

protected:
  bool emitRow(SValue* row, int row_len);
  RowSink* target_;
};

}
}
#endif
