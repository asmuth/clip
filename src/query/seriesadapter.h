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

template <typename T>
class SeriesAdapter {
public:

  SeriesAdapter(T* drawable) : drawable_(drawable) {}

  void addSeries(ResultList* series) {
    if (series->getNumRows() == 0) {
      return;
    }

    int x_ind = series->getColumnIndex("x");
    int y_ind = series->getColumnIndex("y");
    int z_ind = series->getColumnIndex("z");
    int name_ind = series->getColumnIndex("series");

    if (name_ind < 0) {
      RAISE(
          util::RuntimeException,
          "can't draw SELECT because it has no 'series' column");
    }

    if (x_ind < 0) {
      RAISE(
          util::RuntimeException,
          "can't draw SELECT because it has no 'x' column");
    }

    if (y_ind < 0) {
      RAISE(
          util::RuntimeException,
          "can't draw SELECT because it has no 'y' column");
    }

    auto first_row = series->getRow(0);
  }

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
  T* drawable_;
};

}
}
#endif
