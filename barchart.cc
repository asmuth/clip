/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <assert.h>
#include "barchart.h"
#include "canvas.h"
#include "domain.h"

/**
 * todo:
 *  - labels inside/outside
 */
namespace fnordmetric {
namespace ui {

BarChart::BarChart(
    Canvas* canvas,
    NumericalDomain* y_domain /* = nullptr */,
    kBarChartOrientation orientation /* = O_HORIZONTAL */) :
    canvas_(canvas),
    orientation_(orientation),
    y_domain_(y_domain) {}


void BarChart::addSeries(Series2D<std::string, double>* series) {
  for (const auto& point : series->getData()) {
    const auto& x_val = std::get<0>(point);
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

    bar_data->ys.emplace_back(0, y_val);   //scaleValue(&y_val, &y_domain));
  }
}

AxisDefinition* BarChart::addAxis(AxisDefinition::kPosition position) {
  switch (position) {

    case AxisDefinition::TOP:
      switch (orientation_) {
        case O_VERTICAL:
          return canvas_->addAxis(position, getLabelDomain());
          break;
        case O_HORIZONTAL:
          return canvas_->addAxis(position, getValueDomain());
          break;
      }
      break;

    case AxisDefinition::RIGHT:
      switch (orientation_) {
        case O_VERTICAL:
          return canvas_->addAxis(position, getValueDomain());
          break;
        case O_HORIZONTAL:
          return canvas_->addAxis(position, getLabelDomain());
          break;
      }
      break;

    case AxisDefinition::BOTTOM:
      switch (orientation_) {
        case O_VERTICAL:
          return canvas_->addAxis(position, getLabelDomain());
          break;
        case O_HORIZONTAL:
          return canvas_->addAxis(position, getValueDomain());
          break;
      }
      break;

    case AxisDefinition::LEFT:
      switch (orientation_) {
        case O_VERTICAL:
          return canvas_->addAxis(position, getValueDomain());
          break;
        case O_HORIZONTAL:
          return canvas_->addAxis(position, getLabelDomain());
          break;
      }
      break;

  }
}



void BarChart::render(
    RenderTarget* target,
    int width,
    int height,
    std::tuple<int, int, int, int>* padding) const {
  switch (orientation_) {
    case O_VERTICAL:
      //drawVerticalBars(target);
      break;
    case O_HORIZONTAL:
      //drawHorizontalBars(target);
      break;
  }
}

CategoricalDomain* BarChart::getLabelDomain() const {
  if (label_domain_auto_.get() == nullptr) {
    label_domain_auto_.reset(calculateLabelDomain());
  }

  return label_domain_auto_.get();
}

NumericalDomain* BarChart::getValueDomain() const {
  if (y_domain_ != nullptr) {
    return y_domain_;
  }

  if (y_domain_auto_.get() == nullptr) {
    y_domain_auto_.reset(calculateValueDomain());
  }

  return y_domain_auto_.get();
}

NumericalDomain* BarChart::calculateValueDomain() const {
  /* calculate our domain*/
  if (stacked_) {
    /*
    int num_bars = 0;
    for (const auto& series : getSeries()) {
      if (series->getData().size() > num_bars) {
        num_bars = series->getData().size();
      }
    }

    double y_domain_max = 0.0f;
    for (int i = 0; i < num_bars; ++i) {
      int sum = 0;
      for (const auto& series : getSeries()) {
        if (i < series->getData().size()) {
          sum += series->getData()[i][1].getFloat();
        }
      }

      if (sum > y_domain_max) {
        y_domain_max = sum;
      }
    }
    y_domain = Domain(0, y_domain_max, false);
    */
  } else {
    double y_domain_min = 0.0f;
    double y_domain_max = 0.0f;

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

    return new NumericalDomain(y_domain_min, y_domain_max, false);
  }
}

}
}
