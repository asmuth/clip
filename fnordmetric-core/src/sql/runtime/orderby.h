/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_SQL_ORDERBY_H
#define _FNORDMETRIC_SQL_ORDERBY_H
#include <stdlib.h>
#include <string>
#include <string.h>
#include <vector>
#include <fnordmetric/sql/parser/astnode.h>
#include <fnordmetric/sql/parser/token.h>
#include <fnordmetric/sql/runtime/queryplannode.h>

namespace fnordmetric {
namespace query {

class OrderBy : public QueryPlanNode {
public:

  struct SortSpec {
    size_t column_index;
    bool descending; // false == ASCENDING, true == DESCENDING
  };

  OrderBy(
      std::vector<SortSpec> sort_specs,
      QueryPlanNode* child);

  void execute() override;
  bool nextRow(SValue* row, int row_len) override;
  size_t getNumCols() const override;
  const std::vector<std::string>& getColumns() const override;

protected:
  std::vector<SortSpec> sort_specs_;
  QueryPlanNode* child_;
};

}
}
#endif
