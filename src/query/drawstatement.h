/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_QUERY_DRAWSTATEMENT_H
#define _FNORDMETRIC_QUERY_DRAWSTATEMENT_H
#include <stdlib.h>
#include <assert.h>
#include "compile.h"
#include "execute.h"
#include "executable.h"
#include "seriesstatement.h"
#include "axisstatement.h"

namespace fnordmetric {
namespace ui {
class Canvas;
}
namespace query {

class DrawStatement : public Executable {
public:

  enum kDrawStatementType {
    T_BAR_CHART,
    T_LINE_CHART,
    T_AREA_CHART
  };

  DrawStatement(
      kDrawStatementType type) :
      type_(type) {}

  void execute() override {}

  bool nextRow(SValue* row, int row_len) override {
    assert(0);
  }

  size_t getNumCols() const override {
    return 0;
  }

  const std::vector<std::string>& getColumns() const override {
    static std::vector<std::string> cols;
    return cols;
  }

  kDrawStatementType getType() const {
    return type_;
  }

  void addSeriesStatement(SeriesStatement* series_stmt) {
    series_stmts_.push_back(series_stmt);
  }

  void addAxisStatement(AxisStatement* axis_stmt) {
    axis_stmts_.push_back(axis_stmt);
  }

  void execute(ui::Canvas* canvas);

  template <typename T>
  void executeDrawable(T* drawable) {
    for (const auto& series_stmt : series_stmts_) {
      series_stmt->executeDrawable(drawable);
    }

    for (const auto& axis_stmt : axis_stmts_) {
      axis_stmt->executeDrawable(drawable);
    }
  }

protected:
  std::vector<SeriesStatement*> series_stmts_;
  std::vector<AxisStatement*> axis_stmts_;
  kDrawStatementType type_;
};

}
}
#endif
