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
#include "compile.h"
#include "execute.h"
#include "../seriesdefinition.h"

namespace fnordmetric {
namespace query {

class SeriesStatement : public Executable {
public:

  SeriesStatement(
      std::vector<std::string>&& columns,
      Executable* child) :
      columns_(std::move(columns)),
      child_(child) {
    child->setTarget(this);
  }

  void execute() override {}

  bool nextRow(SValue* row, int row_len) override {
    SValue out[128]; // FIXPAUL
    int out_len = 0;

    /*
    executeExpression(
        name_expr_,
        nullptr,
        row_len,
        row,
        &out_len,
        out);
    assert(out_len == 1);

    const auto& series_name = out[0].toString();
    const auto& series_iter = series_.find(series_name);
    SeriesDefinition* series;
    if (series_iter == series_.end()) {
      series = new SeriesDefinition(series_name);
      series_[series_name] = series;
    } else {
      series = series_iter->second;
    }

    // FIXPAUL execute series property expressions
    // FIXPAUL: optimization -- execute all non aggregate exprs only on last row
    // FIXPAUL: optimization -- set props only on last row

    std::vector<SValue> datum;
    assert(row_len >= columns_.size() - 1);
    for (int i = 0; i < columns_.size() - 1; ++i) {
      datum.emplace_back(row[i]);
    }
    series->addDatum(std::move(datum));
    */
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
  Executable* child_;
};

}
}
#endif
