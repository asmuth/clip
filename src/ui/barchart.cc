/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <assert.h>
#include "barchart.h"
#include "canvas.h"
#include "domain.h"
#include "rendertarget.h"

/**
 * todo:
 *  - labels inside/outside
 */
namespace fnordmetric {
namespace ui {

/*
void BarChart::addSeries(Series2D<double, double>* series) {
  series_colors_.emplace_back(seriesColor(series));

  for (const auto& point : series->getData()) {
    const auto& x_val = util::format::numberToHuman(std::get<0>(point)); // FIXPAUL
    const auto& y_val = std::get<1>(point);

    BarData* bar_data = nullptr;

    for (auto& candidate : data_) {
      if (candidate.x == x_val) {
        bar_data = &candidate;
      }
    }

    if (bar_data == nullptr) {
      data_.emplace_back();
      bar_data = &data_.back();
      bar_data->x = x_val;
    }

    if (bar_data->ys.size() < num_series_ + 1) {
      for (int i = bar_data->ys.size(); i < num_series_; ++i) {
        bar_data->ys.emplace_back(0, 0);
      }

      bar_data->ys.emplace_back(0, y_val);
    }
  }

  num_series_++;
}

void BarChart::addSeries(Series3D<std::string, double, double>* series) {
  series_colors_.emplace_back(seriesColor(series));

  for (const auto& point : series->getData()) {
    const auto& x_val = std::get<0>(point);
    const auto& y_val = std::get<1>(point);
    const auto& z_val = std::get<2>(point);

    BarData* bar_data = nullptr;

    for (auto& candidate : data_) {
      if (candidate.x == x_val) {
        bar_data = &candidate;
      }
    }

    if (bar_data == nullptr) {
      data_.emplace_back();
      bar_data = &data_.back();
      bar_data->x = x_val;
    }

    if (bar_data->ys.size() < num_series_ + 1) {
      for (int i = bar_data->ys.size(); i < num_series_; ++i) {
        bar_data->ys.emplace_back(0, 0);
      }

      bar_data->ys.emplace_back(y_val, z_val);
    }
  }

  num_series_++;
}
*/

/*
template <typename TX, typename TY>
NumericalDomain* BarChart<TX, TY>::newYDomain() const {
  double y_domain_min = 0.0f;
  double y_domain_max = 0.0f;

  if (stacked_) {
    for (const auto& group : data_) {
      int sum = 0;
      for (const auto& y : group.ys) {
        sum += y.second;
      }

      if (sum > 0 && sum > y_domain_max) {
        y_domain_max = sum;
      }
    }
  } else {
    for (const auto& group : data_) {
      for (const auto& y : group.ys) {
        if (y.first < y_domain_min) {
          y_domain_min = y.first;
        }
        if (y.first > y_domain_max) {
          y_domain_max = y.first;
        }
        if (y.second < y_domain_min) {
          y_domain_min = y.second;
        }
        if (y.second > y_domain_max) {
          y_domain_max = y.second;
        }
      }
    }
  }

  if (y_domain_max > 0) {
    y_domain_max *= 1.1;
  }

  if (y_domain_max < 0) {
    y_domain_max *= 0.9;
  }

  if (y_domain_min > 0) {
    y_domain_min *= 0.9;
  }

  if (y_domain_min < 0) {
    y_domain_min *= 1.1;
  }

  return new NumericalDomain(y_domain_min, y_domain_max);
}
*/

/*
AxisDefinition* BarChart::newLabelAxis(AxisDefinition::kPosition position)
    const {
  auto axis = canvas_->addAxis(position);
  axis->addTick(0.0f);

  auto bar_width = (1.0 / data_.size()) * (1.0f - kBarPadding);
  auto bar_padding = (1.0 / data_.size()) * (kBarPadding * 0.5f);
  bar_width -= bar_padding / data_.size() * 2;

  auto draw_x = bar_padding;
  for (int i = 0; i < data_.size(); ++i) {
    draw_x += bar_padding;

    switch (orientation_) {
      case O_VERTICAL:
        axis->addLabel(draw_x + bar_width * 0.5f, data_[i].x);
        break;
      case O_HORIZONTAL:
        axis->addLabel(
            draw_x + bar_width * 0.5f,
            data_[data_.size() - 1 - i].x);
        break;
    }

    draw_x += bar_width + bar_padding;
    if (i < data_.size() - 1) {
      axis->addTick(draw_x);
    }
  }

  axis->addTick(1.0f);
  return axis;
}
*/


}
}
