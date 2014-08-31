/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_SERIESJOIN_H
#define _FNORDMETRIC_SERIESJOIN_H
#include <stdlib.h>
#include <string>
#include <vector>

namespace fnordmetric {
namespace ui {

template <typename TX, typename TY, typename TZ>
class SeriesJoin3D {
public:
  struct JoinedPoint {
    JoinedPoint(const Series::Coord<TX>& x_) : x(x_) {}
    Series::Coord<TX> x;
    std::vector<std::pair<Series::Coord<TY>, Series::Coord<TZ>>> ys;
  };

  SeriesJoin3D() : num_series_(0) {}

  // FIXPAUL this should not be O(n^2)
  void addSeries(Series3D<TX, TY, TZ>* series) {
    for (const auto& point : series->getData()) {
      addPoint(point);
    }

    for (auto& joined : data_) {
      for (int i = joined.ys.size(); i <= num_series_; ++i) {
        joined.ys.emplace_back(nullptr, nullptr);
      }
    }

    num_series_++;
  }

  void addPoint(
    const typename Series3D<TX, TY, TZ>::Point& point,
    bool force = false) {
    JoinedPoint* joined = nullptr;
    for (auto& candidate : data_) {
      if (candidate.x == point.x_coord()) {
        joined = &candidate;
      }
    }

    if (joined == nullptr) {
      data_.emplace_back(point.x_coord());
      joined = &data_.back();
    }

    for (int i = joined->ys.size(); i < num_series_; ++i) {
      joined->ys.emplace_back(nullptr, nullptr);
    }

    if (joined->ys.size() < num_series_ + 1 || force) {
      joined->ys.emplace_back(point.y_coord(), point.z_coord());
    }
  }

  size_t size() const {
    return data_.size();
  }

  const std::vector<JoinedPoint>& getData() const {
    return data_;
  }

  const size_t seriesCount() const {
    return num_series_;
  }

  void setSeriesCount(int num_series) {
    num_series_ = num_series;
  }

protected:
  std::vector<JoinedPoint> data_;
  int num_series_;
};


}
}
#endif
