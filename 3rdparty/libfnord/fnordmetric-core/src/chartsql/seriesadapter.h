/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_SQLEXTENSIONS_SERIESADAPTER_H
#define _FNORDMETRIC_SQLEXTENSIONS_SERIESADAPTER_H
#include <stdlib.h>
#include <assert.h>
#include <unordered_map>
#include <fnord-chart/canvas.h>
#include <fnord-chart/barchart.h>
#include <fnord-chart/series.h>
#include <fnord-base/exception.h>
#include <sql/runtime/compile.h>
#include <sql/runtime/execute.h>
#include <sql/runtime/rowsink.h>
#include <sql/runtime/queryplannode.h>

namespace fnordmetric {
namespace query {

class AnySeriesAdapter : public RowSink {
public:

  AnySeriesAdapter(
      int name_ind,
      int x_ind,
      int y_ind,
      int z_ind,
      ResultList* result_list);

  int name_ind_;
  int x_ind_;
  int y_ind_;
  int z_ind_;
  std::vector<std::pair<fnord::chart::Series::kProperty, int>> prop_indexes_;

protected:

  void applyProperties(
      SValue* row,
      int row_len,
      fnord::chart::Series* series,
      fnord::chart::Series::AnyPoint* point);

  void copyToResultList(SValue* row, int row_len);

  ResultList* result_list_;
};

template <typename TX, typename TY>
class SeriesAdapter2D : public AnySeriesAdapter {
public:

  SeriesAdapter2D(
      int name_ind,
      int x_ind,
      int y_ind,
      ResultList* result_list) :
      AnySeriesAdapter(name_ind, x_ind, y_ind, -1, result_list) {}

  bool nextRow(SValue* row, int row_len) override {
    copyToResultList(row, row_len);
    std::string name = "unnamed";

    if (name_ind_ >= 0) {
      name = row[name_ind_].template getValue<std::string>();
    }

    fnord::chart::Series2D<TX, TY>* series = nullptr;
    const auto& series_iter = series_map_.find(name);
    if (series_iter == series_map_.end()) {
      series = new fnord::chart::Series2D<TX, TY>(name);
      series_map_.emplace(name, series);
      series_list_.emplace_back(series);
    } else {
      series = series_iter->second;
    }

    if (row[y_ind_].getType() == SValue::T_NULL) {
      // FIXPAUL better handling of missing/NULL values!
    } else {
      series->addDatum(
          row[x_ind_].template getValue<TX>(),
          row[y_ind_].template getValue<TY>());
    }

    applyProperties(
        row,
        row_len,
        series,
        &series->getData().back());

    return true;
  }

  std::unordered_map<std::string, fnord::chart::Series2D<TX, TY>*> series_map_;
  std::vector<fnord::chart::Series2D<TX, TY>*> series_list_;
};

template <typename TX, typename TY, typename TZ>
class SeriesAdapter3D : public AnySeriesAdapter {
public:

  SeriesAdapter3D(
      int name_ind,
      int x_ind,
      int y_ind,
      int z_ind,
      ResultList* result_list) :
      AnySeriesAdapter(name_ind, x_ind, y_ind, z_ind, result_list) {}

  bool nextRow(SValue* row, int row_len) override {
    copyToResultList(row, row_len);
    std::string name = "unnamed";

    if (name_ind_ >= 0) {
      name = row[name_ind_].template getValue<std::string>();
    }

    fnord::chart::Series3D<TX, TY, TZ>* series = nullptr;
    const auto& series_iter = series_map_.find(name);
    if (series_iter == series_map_.end()) {
      series = new fnord::chart::Series3D<TX, TY, TZ>(name);
      series_map_.emplace(name, series);
      series_list_.emplace_back(series);
    } else {
      series = series_iter->second;
    }

    series->addDatum(
        row[x_ind_].template getValue<TX>(),
        row[y_ind_].template getValue<TY>(),
        row[z_ind_].template getValue<TZ>());

    applyProperties(
        row,
        row_len,
        series,
        &series->getData().back());

    return true;
  }

  std::unordered_map<
      std::string,
      fnord::chart::Series3D<TX, TY, TZ>*> series_map_;

  std::vector<fnord::chart::Series3D<TX, TY, TZ>*> series_list_;
};

}
}
#endif
