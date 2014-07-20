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
#include "../base/series.h"

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

  template <typename T>
  void executeDrawable(T* drawable) {
    child_->execute();

    if (rows_.size() == 0) {
      return;
    }

    int x_ind = -1;
    int y_ind = -1;
    int z_ind = -1;

    const auto& cols = getColumns();
    for (int i = 0; i < getNumCols(); ++i) {
      const auto& col = cols[i];

      if (col == "x" || col == "X") {
        x_ind = i;
        continue;
      }

      if (col == "y" || col == "Y") {
        y_ind = i;
        continue;
      }

      if (col == "z" || col == "Z") {
        z_ind = i;
        continue;
      }
    }

    bool is_2d = x_ind >= 0 && y_ind >= 0;
    bool is_3d = z_ind >= 0 && is_2d;

    if (is_3d) {

    } else if (is_2d) {
      switch (rows_[0][x_ind].getType()) {
        case SValue::T_FLOAT:
        case SValue::T_INTEGER:
        case SValue::T_BOOL:
          switch (rows_[0][y_ind].getType()) {
            case SValue::T_FLOAT:
            case SValue::T_INTEGER:
            case SValue::T_BOOL:
              return executeSeries<T, double, double>(
                  drawable,
                  x_ind,
                  y_ind);

            case SValue::T_STRING:
              return executeSeries<T, double, std::string>(
                  drawable,
                  x_ind,
                  y_ind);

            case SValue::T_UNDEFINED:
              break;
          }

        case SValue::T_STRING:
          switch (rows_[0][y_ind].getType()) {
            case SValue::T_FLOAT:
            case SValue::T_INTEGER:
            case SValue::T_BOOL:
              return executeSeries<T, std::string, double>(
                  drawable,
                  x_ind,
                  y_ind);

            case SValue::T_STRING:
              return executeSeries<T, std::string, std::string>(
                  drawable,
                  x_ind,
                  y_ind);

            case SValue::T_UNDEFINED:
              break;
          }

        case SValue::T_UNDEFINED:
          break;
      }

      throw std::string("undefined SValue");
    } else {
      throw std::string("error: not enough dimensions");
    }
  }

  template <typename T, typename D1, typename D2>
  void executeSeries(T* drawable, int x_ind, int y_ind) {
    auto series = new Series2D<D1, D2>();
    for (const auto& row : rows_) {
      series->addDatum(
          row[x_ind].getValue<D1>(),
          row[x_ind].getValue<D2>());
    }

    drawable->addSeries(series);
  }

  bool nextRow(SValue* row, int row_len) override {
    rows_.emplace_back();

    if (row_len != columns_.size()) {
      throw std::string("invalid row size");
    }

    for (int i = 0; i < row_len; ++i) {
      rows_.back().emplace_back(row[i]);
    }
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
  std::vector<std::vector<SValue>> rows_;
  Executable* child_;
};

}
}
#endif
