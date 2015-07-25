/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _libstx_CONTINUOUSDOMAIN_H
#define _libstx_CONTINUOUSDOMAIN_H
#include "stx/exception.h"
#include "stx/stringutil.h"
#include "cplot/domain.h"

namespace stx {
namespace chart {

class AnyContinuousDomain {
public:
  virtual void setLogarithmic(bool logarithmic) = 0;
  virtual void setPadding(double min_padding, double top_padding) = 0;
};

template <typename T>
class ContinuousDomain : public Domain<T>, public AnyContinuousDomain {
public:

  /**
   * Create a new numerical domain with explicit parameters
   *
   * @param min_value the smallest value
   * @param max_value the largest value
   * @param logarithmic is this domain a logarithmic domain?
   */
  ContinuousDomain(
      T min_value = std::numeric_limits<T>::max(),
      T max_value = std::numeric_limits<T>::min(),
      bool is_logarithmic = false,
      bool is_inverted = false) :
      min_value_(min_value),
      max_value_(max_value),
      is_logarithmic_(is_logarithmic),
      is_inverted_(is_inverted),
      padding_(0, 0) {}

  double scale(T value) const {
    double scaled;

    if (is_logarithmic_) {
      if (min_value_ < 0) {
        RAISE(
            kRuntimeError,
            "negative value is outside of logarithmic domain");
      }

      double max_log = 0.0f;
      if (max_value_ >= 1.0) {
        max_log = log10(max_value_ + max_value_* padding_.second);
      }

      double value_log = 0.0f;
      if (static_cast<double>(value) >= 1.0) {
        value_log = log10(static_cast<double>(value));
      }

      scaled = value_log / max_log;
    } else {
      auto min_max = getRangeWithPadding();
      auto min_value = min_max.first;
      auto max_value = min_max.second;

      scaled =
          (static_cast<double>(value) - min_value) / (max_value - min_value);
    }

    if (is_inverted_) {
      return 1.0 - scaled;
    } else {
      return scaled;
    }
  }

  virtual std::string label(T value) const {
    return StringUtil::formatNumberMetric(static_cast<double>(value));
  }

  double valueAt(double index) const {
    if (is_logarithmic_) {
      if (max_value_ < 0) {
        RAISE(
            kRuntimeError,
            "negative value is outside of logarithmic domain");
      }

      double max_log = 0.0f;
      if (max_value_ >= 1.0) {
        max_log = log10(max_value_ + max_value_* padding_.second);
      }

      if (is_inverted_) {
        return pow(10, (1.0 - index) * max_log);
      } else {
        return pow(10, index * max_log);
      }
    } else {
      auto min_max = getRangeWithPadding();
      double min_value = min_max.first;
      double max_value = min_max.second;
      double val_range = min_value + (max_value - min_value);

      if (is_inverted_) {
        return min_value + (max_value - min_value) * (1.0 - index);
      } else {
        return min_value + (max_value - min_value) * index;
      }
    }
  }

  std::pair<double, double> scaleRange(T value) const {
    return std::make_pair(scale(value), scale(value));
  }

  void addValue(const T& value) {
    if (static_cast<double>(value) > max_value_) {
      max_value_ = static_cast<double>(value);
    }

    if (static_cast<double>(value) < min_value_) {
      min_value_ = static_cast<double>(value);
    }
  }

  bool contains(T value) const {
    return false;
  }

  const std::vector<double> getTicks() const {
    std::vector<double> ticks;

    double num_ticks = AnyDomain::kDefaultNumTicks;
    for (int n = 0; n < num_ticks; ++n) {
      ticks.push_back((double) n / (double) (num_ticks - 1));
    }

    return ticks;
  }

  const std::vector<std::pair<double, std::string>> getLabels() const {
    auto ticks = getTicks();
    std::vector<std::pair<double, std::string>> labels;

    for (auto tick : ticks) {
      labels.emplace_back(tick, label(valueAt(tick)));
    }

    return labels;
  }

  void setMin(T min) {
    min_value_ = static_cast<double>(min);
    padding_.first = 0.0f;
  }

  void setMax(T max) {
    max_value_ = static_cast<double>(max);
    padding_.second = 0.0f;
  }

  void setInverted(bool inverted) {
    is_inverted_ = inverted;
  }

  void setLogarithmic(bool logarithmic) {
    is_logarithmic_ = logarithmic;
  }

  void setPadding(double min_padding, double max_padding) {
    padding_.first = min_padding;
    padding_.second = max_padding;
  }

  void build() {
    if (min_value_ == max_value_) {
      max_value_ += 1.0f;
      min_value_ -= 1.0f;
    }
  }

protected:

  std::pair<double, double> getRangeWithPadding() const {
    double range = max_value_ - min_value_;

    return std::make_pair(
        min_value_ == 0 ? 0 : min_value_ - range * padding_.first,
        max_value_ + range * padding_.second);
  }

  T getRange() const {
    return max_value_ - min_value_;
  }

  double min_value_;
  double max_value_;
  bool is_logarithmic_;
  bool is_inverted_;
  std::pair<double, double> padding_;
};

}
}
#endif
