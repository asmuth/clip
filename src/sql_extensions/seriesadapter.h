/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_QUERY_SERIESADAPTER_H
#define _FNORDMETRIC_QUERY_SERIESADAPTER_H
#include <stdlib.h>
#include <assert.h>
#include <unordered_map>
#include <fnordmetric/base/series.h>
#include <fnordmetric/ui/canvas.h>
#include <fnordmetric/ui/barchart.h>
#include <fnordmetric/util/runtimeexception.h>
#include <fnordmetric/sql/compile.h>
#include <fnordmetric/sql/execute.h>

namespace fnordmetric {
namespace query {

class AbstractSeriesAdapter : public RowSink {
public:

  AbstractSeriesAdapter(
      int name_ind,
      int x_ind,
      int y_ind,
      int z_ind) :
      name_ind_(name_ind),
      x_ind_(x_ind),
      y_ind_(y_ind),
      z_ind_(z_ind) {}

  virtual ui::Drawable* getDrawable() = 0;

  int name_ind_;
  int x_ind_;
  int y_ind_;
  int z_ind_;
};

template <typename TX, typename TY>
class SeriesAdapter2D : public AbstractSeriesAdapter {
public:

  SeriesAdapter2D(
      ui::Canvas* canvas,
      int name_ind,
      int x_ind,
      int y_ind) :
      canvas_(canvas),
      AbstractSeriesAdapter(name_ind, x_ind, y_ind, -1) {}

  bool nextRow(SValue* row, int row_len) override {
    std::string name = "unnamed";

    if (name_ind_ >= 0) {
      name = row[name_ind_].template getValue<std::string>();
    }

    Series2D<TX, TY>* series = nullptr;
    const auto& series_iter = series_map_.find(name);
    if (series_iter == series_map_.end()) {
      series = new Series2D<TX, TY>(name);
      series_map_.emplace(name, series);
      series_list_.emplace_back(series);
    } else {
      series = series_iter->second;
    }

    series->addDatum(
        row[x_ind_].template getValue<TX>(),
        row[y_ind_].template getValue<TY>());

    return true;
  }

  ui::Drawable* getDrawable() override {
    auto x_domain = new ui::CategoricalDomain<TX>(); // FIXPAUL domain builder
    auto y_domain = new ui::NumericalDomain<TY>(0, 100); // FIXPAUL domain builder
    auto drawable = canvas_->addChart2D<ui::BarChart<TX, TY>>(x_domain, y_domain);

    for (const auto& series : series_list_) {
      drawable->addSeries(static_cast<Series2D<TX, TY>*>(series.get()));
    }

    return drawable;
  }

protected:
  ui::Canvas* canvas_;
  std::unordered_map<std::string, Series2D<TX, TY>*> series_map_;
  std::vector<std::unique_ptr<Series2D<TX, TY>>> series_list_;
};

class SeriesAdapter : public RowSink {
public:

  SeriesAdapter(ui::Canvas* canvas) : canvas_(canvas) {}

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
        RAISE(
            util::RuntimeException,
            "3D series not implemented yet");
      }
    } else {
      adapter_->name_ind_ = name_ind_;
      adapter_->x_ind_ = x_ind_;
      adapter_->y_ind_ = y_ind_;
      adapter_->z_ind_ = y_ind_;
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

  std::unique_ptr<AbstractSeriesAdapter> adapter_;

protected:

  AbstractSeriesAdapter* mkSeriesAdapter2D(SValue* x, SValue* y) {
    if (testSeriesSchema2D<double, double>(x, y)) {
      return new SeriesAdapter2D<double, double>(
          canvas_,
          name_ind_,
          x_ind_,
          y_ind_);
    }

    if (testSeriesSchema2D<std::string, double>(x, y)) {
      return new SeriesAdapter2D<std::string, double>(
          canvas_,
          name_ind_,
          x_ind_,
          y_ind_);
    }

    return new SeriesAdapter2D<std::string, std::string>(
        canvas_,
        name_ind_,
        x_ind_,
        y_ind_);
  }

  template <typename TX, typename TY>
  bool testSeriesSchema2D(SValue* x, SValue* y) const {
    return (x->testType<TX>() && y->testType<TY>());
  }

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
