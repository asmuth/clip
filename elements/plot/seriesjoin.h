/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _libstx_SERIESJOIN_H
#define _libstx_SERIESJOIN_H
#include <stdlib.h>
#include <string>
#include <vector>

namespace signaltk {
namespace chart {

template <typename TX, typename TY, typename TZ>
class SeriesJoin3D {
public:
  struct JoinedPoint {
    JoinedPoint(const Series::Coord<TX>& x_) : x(x_) {}
    Series::Coord<TX> x;
    std::string label;
    std::vector<std::pair<Series::Coord<TY>, Series::Coord<TZ>>> ys;
  };

  SeriesJoin3D() : num_series_(0) {}

  // FIXPAUL this should not be O(n^2)
  void addSeries(Series3D<TX, TY, TZ>* series) {
    for (const auto& point : series->getData()) {
      addPoint(point, series->labelFor(&point));
    }

    for (auto& joined : data_) {
      for (int i = joined.ys.size(); i <= num_series_; ++i) {
        joined.ys.emplace_back(
            Series::Coord<TY>(nullptr),
            Series::Coord<TZ>(nullptr));
      }
    }

    num_series_++;
  }

  void addPoint(
      const typename Series3D<TX, TY, TZ>::Point& point,
      const std::string& label,
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
      joined->ys.emplace_back(
          Series::Coord<TY>(nullptr),
          Series::Coord<TZ>(nullptr));
    }

    if (joined->ys.size() < num_series_ + 1 || force) {
      joined->ys.emplace_back(point.y_coord(), point.z_coord());
    }

    if (joined->label.size() == 0) {
      joined->label = label;
    } else if (joined->label != label) {
      joined->label.append(", ");
      joined->label.append(label);
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
