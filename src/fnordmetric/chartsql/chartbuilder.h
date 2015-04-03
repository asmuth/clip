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
#include <fnord-base/exception.h>
#include <fnord-chart/canvas.h>
#include <fnord-chart/barchart.h>
#include <fnord-chart/series.h>
#include <fnordmetric/sql/runtime/compile.h>
#include <fnordmetric/sql/runtime/execute.h>
#include <fnordmetric/sql/runtime/rowsink.h>
#include <fnordmetric/sql/runtime/resultlist.h>
#include <fnordmetric/sql/runtime/queryplannode.h>
#include <fnordmetric/chartsql/seriesadapter.h>

namespace fnordmetric {
namespace query {
class DrawStatement;

class ChartBuilder : public RowSink {
public:

  ChartBuilder(
      fnord::chart::Canvas* canvas,
      DrawStatement const* draw_stmt) :
      canvas_(canvas),
      draw_stmt_(draw_stmt) {}

  bool nextRow(SValue* row, int row_len) {
    if (x_ind_ < 0) {
      RAISE(
          kRuntimeError,
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

  void executeStatement(QueryPlanNode* stmt, ResultList* result_list) {
    name_ind_ = stmt->getColumnIndex("series");

    x_ind_ = stmt->getColumnIndex("x");
    if (x_ind_ < 0) {
      x_ind_ = stmt->getColumnIndex("X");
    }

    y_ind_ = stmt->getColumnIndex("y");
    if (y_ind_ < 0) {
      y_ind_ = stmt->getColumnIndex("Y");
    }

    z_ind_ = stmt->getColumnIndex("z");
    if (z_ind_ < 0) {
      z_ind_ = stmt->getColumnIndex("Z");
    }

    prop_indexes_.clear();

    int color_ind = stmt->getColumnIndex("color");
    if (color_ind >= 0) {
      prop_indexes_.emplace_back(fnord::chart::Series::P_COLOR, color_ind);
    }

    int label_ind = stmt->getColumnIndex("label");
    if (label_ind >= 0) {
      prop_indexes_.emplace_back(fnord::chart::Series::P_LABEL, label_ind);
    }

    int line_style_ind = stmt->getColumnIndex("linestyle");
    if (line_style_ind >= 0) {
      prop_indexes_.emplace_back(
          fnord::chart::Series::P_LINE_STYLE,
          line_style_ind);
    }

    int line_width_ind = stmt->getColumnIndex("linewidth");
    if (line_width_ind >= 0) {
      prop_indexes_.emplace_back(
          fnord::chart::Series::P_LINE_WIDTH,
          line_width_ind);
    }

    int point_style_ind = stmt->getColumnIndex("pointstyle");
    if (point_style_ind >= 0) {
      prop_indexes_.emplace_back(
          fnord::chart::Series::P_POINT_STYLE,
          point_style_ind);
    }

    int point_size_ind = stmt->getColumnIndex("pointsize");
    if (point_size_ind >= 0) {
      prop_indexes_.emplace_back(
          fnord::chart::Series::P_POINT_SIZE,
          point_size_ind);
    }

    stmt_ = stmt;
    result_list_ = result_list;
    stmt->setTarget(this);
    stmt->execute();
  }

  virtual fnord::chart::Drawable* getChart() const = 0;
  virtual std::string chartName() const = 0;

protected:

  AnySeriesAdapter* mkSeriesAdapter(SValue* row) {
    AnySeriesAdapter* a = nullptr;
    if (!a) a = mkSeriesAdapter1D<fnordmetric::TimeType>(row);
    if (!a) a = mkSeriesAdapter1D<fnordmetric::FloatType>(row);
    if (!a) a = mkSeriesAdapter1D<fnordmetric::StringType>(row);

    if (a == nullptr) {
      RAISE(kRuntimeError, "can't build seriesadapter");
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
    if (!a) a = mkSeriesAdapter2D<TX, fnordmetric::TimeType>(row);
    if (!a) a = mkSeriesAdapter2D<TX, fnordmetric::FloatType>(row);
    if (!a) a = mkSeriesAdapter2D<TX, fnordmetric::StringType>(row);
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
          y_ind_,
          result_list_);
    }

    AnySeriesAdapter* a = nullptr;
    if (!a) a = mkSeriesAdapter3D<TX, TY, fnordmetric::TimeType>(row);
    if (!a) a = mkSeriesAdapter3D<TX, TY, fnordmetric::FloatType>(row);
    if (!a) a = mkSeriesAdapter3D<TX, TY, fnordmetric::StringType>(row);
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
        z_ind_,
        result_list_);
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
        chart->addSeries(series);
      }

      return chart;
    }
  }

  void preconditionCheck() const {
    if (adapter_.get() == nullptr) {
      RAISE(
          kRuntimeError,
          "can't DRAW %s because the query returned no result rows",
          chartName().c_str());
    }
  }

  void invalidType() const {
    RAISE(
        kRuntimeError,
        "invalid series type for %s",
        chartName().c_str()); // FIXPAUL
  }

  std::unique_ptr<AnySeriesAdapter> adapter_;
  int name_ind_;
  int x_ind_;
  int y_ind_;
  int z_ind_;
  std::vector<std::pair<fnord::chart::Series::kProperty, int>> prop_indexes_;
  QueryPlanNode* stmt_;
  ResultList* result_list_;
  fnord::chart::Canvas* canvas_;
  DrawStatement const* draw_stmt_;
};


}
}
#endif
