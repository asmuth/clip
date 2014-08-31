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
class DrawStatement;

class ChartBuilder : public RowSink {
public:

  ChartBuilder(
      ui::Canvas* canvas,
      DrawStatement const* draw_stmt) :
      canvas_(canvas),
      draw_stmt_(draw_stmt) {}

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

    if (adapter_.get() == nullptr) {
      adapter_.reset(mkSeriesAdapter(row));
    } else {
      adapter_->name_ind_ = name_ind_;
      adapter_->x_ind_ = x_ind_;
      adapter_->y_ind_ = y_ind_;
      adapter_->z_ind_ = z_ind_;
    }

    adapter_->prop_indexes_ = prop_indexes_;
    adapter_->nextRow(row, row_len);
    stmt_->setTarget(adapter_.get());
    return true;
  }

  void executeStatement(QueryPlanNode* stmt) {
    name_ind_ = stmt->getColumnIndex("series");
    x_ind_ = stmt->getColumnIndex("x");
    y_ind_ = stmt->getColumnIndex("y");
    z_ind_ = stmt->getColumnIndex("z");

    prop_indexes_.clear();

    int color_ind = stmt->getColumnIndex("color");
    if (color_ind >= 0) {
      prop_indexes_.emplace_back(Series::P_COLOR, color_ind);
    }

    int label_ind = stmt->getColumnIndex("label");
    if (label_ind >= 0) {
      prop_indexes_.emplace_back(Series::P_LABEL, label_ind);
    }

    stmt_ = stmt;
    stmt->setTarget(this);
    stmt->execute();
  }

  virtual ui::Drawable* getChart() const = 0;
  virtual std::string chartName() const = 0;

protected:

  AnySeriesAdapter* mkSeriesAdapter(SValue* row) {
    AnySeriesAdapter* a = nullptr;
    if (!a) a = mkSeriesAdapter1D<int>(row);
    if (!a) a = mkSeriesAdapter1D<double>(row);
    if (!a) a = mkSeriesAdapter1D<std::string>(row);

    if (a == nullptr) {
      RAISE(util::RuntimeException, "can't build seriesadapter");
    }

    return a;
  }

  template <typename TX>
  AnySeriesAdapter* mkSeriesAdapter1D(SValue* row) {
    if (!row[x_ind_].testType<TX>()) {
      return nullptr;
    }

    if (y_ind_ < 0) {
      return nullptr;
    }

    AnySeriesAdapter* a = nullptr;
    if (!a) a = mkSeriesAdapter2D<TX, int>(row);
    if (!a) a = mkSeriesAdapter2D<TX, double>(row);
    if (!a) a = mkSeriesAdapter2D<TX, std::string>(row);
    return a;
  }

  template <typename TX, typename TY>
  AnySeriesAdapter* mkSeriesAdapter2D(SValue* row) {
    if (!row[y_ind_].testType<TY>()) {
      return nullptr;
    }

    if (z_ind_ < 0) {
      return new SeriesAdapter2D<TX, TY>(
          name_ind_,
          x_ind_,
          y_ind_);
    }

    AnySeriesAdapter* a = nullptr;
    if (!a) a = mkSeriesAdapter3D<TX, TY, int>(row);
    if (!a) a = mkSeriesAdapter3D<TX, TY, double>(row);
    if (!a) a = mkSeriesAdapter3D<TX, TY, std::string>(row);
    return a;
  }

  template <typename TX, typename TY, typename TZ>
  AnySeriesAdapter* mkSeriesAdapter3D(SValue* row) {
    if (!row[z_ind_].testType<TZ>()) {
      return nullptr;
    }

    return new SeriesAdapter3D<TX, TY, TZ>(
        name_ind_,
        x_ind_,
        y_ind_,
        z_ind_);
  }

  template <typename T>
  T* tryType2D() const {
    return tryTypeND<T, SeriesAdapter2D<
        typename T::TX,
        typename T::TY>>();
  }

  template <typename T>
  T* tryType3D() const {
    return tryTypeND<T, SeriesAdapter3D<
        typename T::TX,
        typename T::TY,
        typename T::TZ>>();
  }

  template <typename T, typename S>
  T* tryTypeND() const {
    auto adapter = dynamic_cast<S*>(adapter_.get());

    if (adapter == nullptr) {
      return nullptr;
    } else {
      auto chart = canvas_->addChart<T>();

      for (const auto& series : adapter->series_list_) {
        chart->addSeries(series.get());
      }

      return chart;
    }
  }

  void invalidType() const {
    RAISE(
        util::RuntimeException,
        "invalid series type for %s",
        chartName().c_str()); // FIXPAUL
  }

  std::unique_ptr<AnySeriesAdapter> adapter_;
  int name_ind_;
  int x_ind_;
  int y_ind_;
  int z_ind_;
  std::vector<std::pair<Series::kProperty, int>> prop_indexes_;
  QueryPlanNode* stmt_;
  ui::Canvas* canvas_;
  DrawStatement const* draw_stmt_;
};


}
}
#endif
