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
      CompiledExpression* name_expr,
      Executable* child) :
      columns_(std::move(columns)),
      name_expr_(name_expr),
      child_(child) {
    child->setTarget(this);
  }

  void execute() override {
    child_->execute();

    if (target_ != nullptr) {
      for (auto& pair : series_) {
        auto& series = pair.second;
        SValue series_name(pair.first);

        for (const auto& row : series->getData()) {
          std::vector<SValue> out;
          out.emplace_back(series_name);

          for (const auto& col : row) {
            out.emplace_back(col);
          }

          emitRow(out.data(), out.size());
        }
      }
    }
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

    /* add the row to the series */
    std::vector<SValue> datum;
    assert(row_len >= columns_.size() - 1);
    for (int i = 0; i < columns_.size() - 1; ++i) {
      datum.emplace_back(row[i]);
    }
    series->addDatum(std::move(datum));
    return true;
  }

  size_t getNumCols() const override {
    return columns_.size();
  }

  const std::vector<std::string>& getColumns() const override {
    return columns_;
  }

  const std::vector<SeriesDefinition*> getSeries() const {
    std::vector<SeriesDefinition*> series_list;

    for (const auto& pair : series_) {
      series_list.push_back(pair.second);
    }

    return series_list;
  }

protected:
  std::unordered_map<std::string, SeriesDefinition*> series_;
  std::vector<std::string> columns_;
  CompiledExpression* name_expr_;
  Executable* child_;
};

}
}
#endif
