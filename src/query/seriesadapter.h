/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
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

  void finish() {
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
      Executable* stmt) :
      drawable_(drawable),
      stmt_(stmt) {}

  bool nextRow(SValue* row, int row_len) {
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

/*
  template <typename TX, typename TY>
  void copySeries2D(
      ResultList* series,
      int name_ind,
      int x_ind,
      int y_ind) {
    std::unordered_map<std::string, Series2D<D1, D2>*> series_map;
    std::vector<Series2D<D1, D2>*> series_list;

    for (int i = 0; i < series->getNumRows(); ++i) {
      const auto& row = series->getRow(i);
      std::string name = "unnamed";

      if (name_ind >= 0) {
        name = row[name_ind].getValue<std::string>();
      }

      Series2D<D1, D2>* series = nullptr;
      const auto& series_iter = series_map.find(name);
      if (series_iter == series_map.end()) {
        series = new Series2D<D1, D2>(name);
        series_map[name] = series;
        series_list.push_back(series);
      } else {
        series = series_iter->second;
      }

      series->addDatum(
          row[x_ind].getValue<D1>(),
          row[y_ind].getValue<D2>());
    }

    for (auto series : series_list) {
      drawable->addSeries(series);
    }
  }

  template <typename TV>
  bool testType(std::string value) const {
    return true;
  }

  template <typename TV>
  TV castType(std::string value) const;
*/
/*

  void executeDrawable( {
    child_->execute();

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

      if (strncasecmp(col.c_str(), "name", 4) == 0) {
        name_ind = i;
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
                  y_ind,
                  name_ind);

            case SValue::T_STRING:
              return executeSeries<T, double, std::string>(
                  drawable,
                  x_ind,
                  y_ind,
                  name_ind);

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
                  y_ind,
                  name_ind);

            case SValue::T_STRING:
              return executeSeries<T, std::string, std::string>(
                  drawable,
                  x_ind,
                  y_ind,
                  name_ind);

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
  void executeSeries(T* drawable, int x_ind, int y_ind, int name_ind) {
    std::unordered_map<std::string, Series2D<D1, D2>*> series_map;
    std::vector<Series2D<D1, D2>*> series_list;

    for (const auto& row : rows_) {
      std::string name = "unnamed";

      if (name_ind >= 0) {
        name = row[name_ind].getValue<std::string>();
      }

      Series2D<D1, D2>* series = nullptr;
      const auto& series_iter = series_map.find(name);
      if (series_iter == series_map.end()) {
        series = new Series2D<D1, D2>(name);
        series_map[name] = series;
        series_list.push_back(series);
      } else {
        series = series_iter->second;
      }

      series->addDatum(
          row[x_ind].getValue<D1>(),
          row[y_ind].getValue<D2>());
    }

    for (auto series : series_list) {
      drawable->addSeries(series);
    }
  }

  template <typename T, typename D1, typename D2, typename D3>
  void executeSeries(T* drawable, int x_ind, int y_ind, int z_ind) {
    auto series = new Series3D<D1, D2, D3>();
    for (const auto& row : rows_) {
      series->addDatum(
          row[x_ind].getValue<D1>(),
          row[y_ind].getValue<D2>(),
          row[z_ind].getValue<D3>());
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
    return true;
  }

*/
protected:
  std::unique_ptr<AbstractSeriesAdapter> adapter_;
  T* drawable_;
  Executable* stmt_;
  int name_ind_;
  int x_ind_;
  int y_ind_;
  int z_ind_;
};

}
}
#endif
