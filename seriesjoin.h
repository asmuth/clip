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
    JoinedPoint(const Series::Point<TX>& x_) : x(x_) {}
    Series::Point<TX> x;
    std::vector<std::pair<Series::Point<TY>, Series::Point<TZ>>> ys;
  };

  SeriesJoin3D() : num_series_(0) {}

  // FIXPAUL this should not be O(n^2)
  void addSeries(Series3D<TX, TY, TZ>* series) {
    for (const auto& point : series->getData()) {
      const auto& x_val = std::get<0>(point);
      const auto& y_val = std::get<1>(point);
      const auto& z_val = std::get<2>(point);

      JoinedPoint* joined = nullptr;
      for (auto& candidate : data_) {
        if (candidate.x == x_val) {
          joined = &candidate;
        }
      }

      if (joined == nullptr) {
        data_.emplace_back(x_val);
        joined = &data_.back();
      }

      for (int i = joined->ys.size(); i < num_series_; ++i) {
        joined->ys.emplace_back(nullptr, nullptr);
      }

      if (joined->ys.size() < num_series_ + 1) {
        joined->ys.emplace_back(y_val, z_val);
      }
    }

    for (auto& joined : data_) {
      for (int i = joined.ys.size(); i <= num_series_; ++i) {
        joined.ys.emplace_back(nullptr, nullptr);
      }
    }

    num_series_++;
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

protected:
  std::vector<JoinedPoint> data_;
  int num_series_;
};


}
}
#endif
