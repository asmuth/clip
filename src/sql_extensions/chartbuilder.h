/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_SQLEXTENSIONS_CHARTBUILDER_H
#define _FNORDMETRIC_SQLEXTENSIONS_CHARTBUILDER_H
#include <stdlib.h>
#include <assert.h>
#include <unordered_map>
#include <fnordmetric/base/series.h>
#include <fnordmetric/ui/canvas.h>
#include <fnordmetric/ui/barchart.h>
#include <fnordmetric/util/runtimeexception.h>
#include <fnordmetric/sql/compile.h>
#include <fnordmetric/sql/execute.h>
#include <fnordmetric/sql/rowsink.h>
#include <fnordmetric/sql/queryplannode.h>
#include <fnordmetric/sql_extensions/seriesadapter.h>

namespace fnordmetric {
namespace query {

class ChartBuilder : public RowSink {
public:

  ChartBuilder(ui::Canvas* canvas) : canvas_(canvas) {}

  bool nextRow(SValue* row, int row_len) {
    if (name_ind_ < 0) {
      RAISE(
          util::RuntimeException,
          "can't draw SELECT because it has no 'series' column");
    }

    if (x_ind_ < 0) {
      RAISE(
          util::RuntimeException,
          "can't draw SELECT because it has no 'x' column");
    }

    if (y_ind_ < 0) {
      RAISE(
          util::RuntimeException,
          "can't draw SELECT because it has no 'y' column");
    }

    if (adapter_.get() == nullptr) {
      if (z_ind_ < 0) {
        adapter_.reset(mkSeriesAdapter2D(row + x_ind_, row + y_ind_));
      } else {
        adapter_.reset(mkSeriesAdapter3D(
            row + x_ind_,
            row + y_ind_,
            row + z_ind_));
      }
    } else {
      adapter_->name_ind_ = name_ind_;
      adapter_->x_ind_ = x_ind_;
      adapter_->y_ind_ = y_ind_;
      adapter_->z_ind_ = z_ind_;
    }

    adapter_->nextRow(row, row_len);
    stmt_->setTarget(adapter_.get());
    return true;
  }

  void executeStatement(QueryPlanNode* stmt) {
    name_ind_ = stmt->getColumnIndex("series");
    x_ind_ = stmt->getColumnIndex("x");
    y_ind_ = stmt->getColumnIndex("y");
    z_ind_ = stmt->getColumnIndex("z");

    stmt_ = stmt;
    stmt->setTarget(this);
    stmt->execute();
  }

  virtual ui::Drawable* getChart() const = 0;

protected:

  // FIXPAUL: this should be generated!
  AnySeriesAdapter* mkSeriesAdapter2D(SValue* x, SValue* y) {
    if (testSeriesSchema2D<double, double>(x, y)) {
      return new SeriesAdapter2D<double, double>(
          name_ind_,
          x_ind_,
          y_ind_);
    }

    if (testSeriesSchema2D<std::string, double>(x, y)) {
      return new SeriesAdapter2D<std::string, double>(
          name_ind_,
          x_ind_,
          y_ind_);
    }

    return new SeriesAdapter2D<std::string, std::string>(
        name_ind_,
        x_ind_,
        y_ind_);

    assert(0);
  }

  // FIXPAUL: this should be generated!
  AnySeriesAdapter* mkSeriesAdapter3D(SValue* x, SValue* y, SValue* z) {
    if (testSeriesSchema3D<std::string, double, double>(x, y, z)) {
      return new SeriesAdapter3D<std::string, double, double>(
          name_ind_,
          x_ind_,
          y_ind_,
          z_ind_);
    }

    assert(0);
  }

  template <typename TX, typename TY>
  bool testSeriesSchema2D(SValue* x, SValue* y) const {
    return (x->testType<TX>() && y->testType<TY>());
  }

  template <typename TX, typename TY, typename TZ>
  bool testSeriesSchema3D(SValue* x, SValue* y, SValue* z) const {
    return (x->testType<TX>() && y->testType<TY>() && y->testType<TZ>());
  }

  template <typename T>
  T* tryType3D() const {
    auto adapter = dynamic_cast<SeriesAdapter3D<
        typename T::TX,
        typename T::TY,
        typename T::TZ>*>(
        adapter_.get());

    if (adapter == nullptr) {
      return nullptr;
    } else {
      auto chart = new T(canvas_);

      for (const auto& series : adapter->series_list_) {
        chart->addSeries(series.get());
      }

      return chart;
    }
  }

  void invalidType() const {
    RAISE(util::RuntimeException, "invalid series type for BarChart"); // FIXPAUL
  }

  std::unique_ptr<AnySeriesAdapter> adapter_;
  int name_ind_;
  int x_ind_;
  int y_ind_;
  int z_ind_;
  QueryPlanNode* stmt_;
  ui::Canvas* canvas_;
};


}
}
#endif
