/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_QUERY_DRAWSTATEMENT_H
#define _FNORDMETRIC_QUERY_DRAWSTATEMENT_H
#include <stdlib.h>
#include <fnordmetric/util/runtimeexception.h>
#include <fnordmetric/query/axisstatement.h>
#include <fnordmetric/query/compile.h>
#include <fnordmetric/query/execute.h>
#include <fnordmetric/query/executable.h>
#include <fnordmetric/query/resultlist.h>
#include <fnordmetric/query/seriesadapter.h>

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
    RAISE(util::RuntimeException, "DrawStatement#nextRow called");
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

  void addSelectStatement(Executable* select_stmt) {
    select_stmts_.push_back(select_stmt);
  }

  void addAxisStatement(AxisStatement* axis_stmt) {
    axis_stmts_.push_back(axis_stmt);
  }

  void execute(ui::Canvas* canvas);

  template <typename T>
  void executeDrawable(T* drawable) {
    for (const auto& stmt : select_stmts_) {
      SeriesAdapter<T> series_adapter(drawable, stmt);
      stmt->setTarget(&series_adapter);
      stmt->execute();
    }

    for (const auto& axis_stmt : axis_stmts_) {
      axis_stmt->executeDrawable(drawable);
    }
  }

protected:
  std::vector<Executable*> select_stmts_;
  std::vector<AxisStatement*> axis_stmts_;
  kDrawStatementType type_;
};

}
}
#endif
