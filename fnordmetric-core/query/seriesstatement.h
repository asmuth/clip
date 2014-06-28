/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_QUERY_SERIESSTATEMENT_H
#define _FNORDMETRIC_QUERY_SERIESSTATEMENT_H
#include <stdlib.h>
#include <assert.h>

namespace fnordmetric {
namespace query {

class SeriesStatement : public Executable {
public:

  SeriesStatement(
      std::vector<std::string>&& columns,
      CompiledExpression* name_expr,
      Executable* child) :
      columns_(std::move(columns)),
      name_expr_(name_expr),
      child_(child) {
    child->setTarget(this);
  }

  void execute() override {
    child_->execute();
  }

  bool nextRow(SValue* row, int row_len) override {
    SValue out[128]; // FIXPAUL
    int out_len = 0;

    /* execute name expresion and prepend name to row */
    executeExpression(
        name_expr_,
        nullptr,
        row_len,
        row,
        &out_len,
        out);
    assert(out_len == 1);

    /* pass through the remainder of the row */
    assert(row_len >= columns_.size() - 1);
    for (int i = 0; i < columns_.size() - 1; ++i) {
      out[out_len++] = row[i];
    }

    emitRow(out, out_len);
    return true;
  }

  size_t getNumCols() const override {
    return columns_.size();
  }

  const std::vector<std::string>& getColumns() const override {
    return columns_;
  }

protected:

  std::vector<std::string> columns_;
  CompiledExpression* name_expr_;
  Executable* child_;
};

}
}
#endif
