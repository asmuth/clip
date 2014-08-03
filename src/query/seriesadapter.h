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
#include <fnordmetric/util/runtimeexception.h>
#include <fnordmetric/query/compile.h>
#include <fnordmetric/query/execute.h>

namespace fnordmetric {
namespace query {

class AbstractSeriesAdapter : public RowSink {
public:

};

template <typename T, typename TX, typename TY>
class SeriesAdapter2D : public AbstractSeriesAdapter {
public:

  SeriesAdapter2D(
      T* drawable,
      int name_ind,
      int x_ind,
      int y_ind) :
      drawable_(drawable),
      name_ind_(name_ind),
      x_ind_(x_ind),
      y_ind_(y_ind) {}

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

  void finish() override {
    for (const auto& series : series_list_) {
      drawable_->addSeries(series.get());
    }
  }

protected:
  T* drawable_;
  int name_ind_;
  int x_ind_;
  int y_ind_;
  std::unordered_map<std::string, Series2D<TX, TY>*> series_map_;
  std::vector<std::unique_ptr<Series2D<TX, TY>>> series_list_;
};

template <typename T>
class SeriesAdapter : public RowSink {
public:

  SeriesAdapter(
      T* drawable,
      Executable* stmt,
      ResultList* result_list) :
      drawable_(drawable),
      stmt_(stmt),
      result_list_(result_list) {}

  bool nextRow(SValue* row, int row_len) {
    if (result_list_ != nullptr) {
      result_list_->nextRow(row, row_len);
    }

    x_ind_ = stmt_->getColumnIndex("x");
    y_ind_ = stmt_->getColumnIndex("y");
    z_ind_ = stmt_->getColumnIndex("z");
    name_ind_ = stmt_->getColumnIndex("series");

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

    if (z_ind_ < 0) {
      auto adapter = mkSeriesAdapter2D(row + x_ind_, row + y_ind_);
      adapter->nextRow(row, row_len);
      stmt_->setTarget(adapter);
      adapter_.reset(adapter);
    }

    return true;
  }

protected:

  AbstractSeriesAdapter* mkSeriesAdapter2D(SValue* x, SValue* y) {
    if (testSeriesSchema2D<double, double>(x, y)) {
      return new SeriesAdapter2D<T, double, double>(
          drawable_,
          name_ind_,
          x_ind_,
          y_ind_);
    }

    if (testSeriesSchema2D<std::string, double>(x, y)) {
      return new SeriesAdapter2D<T, std::string, double>(
          drawable_,
          name_ind_,
          x_ind_,
          y_ind_);
    }

    return new SeriesAdapter2D<T, std::string, std::string>(
        drawable_,
        name_ind_,
        x_ind_,
        y_ind_);
  }

  template <typename TX, typename TY>
  bool testSeriesSchema2D(SValue* x, SValue* y) const {
    return (x->testType<TX>() && y->testType<TY>());
  }

protected:
  std::unique_ptr<AbstractSeriesAdapter> adapter_;
  T* drawable_;
  Executable* stmt_;
  ResultList* result_list_;
  int name_ind_;
  int x_ind_;
  int y_ind_;
  int z_ind_;
};

}
}
#endif
