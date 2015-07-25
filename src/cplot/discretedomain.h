/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _libstx_DISCRETEDOMAIN_H
#define _libstx_DISCRETEDOMAIN_H
#include "stx/stringutil.h"
#include "cplot/domain.h"

namespace stx {
namespace chart {

template <typename T>
class DiscreteDomain : public Domain<T> {
public:

  /**
   * Create a new categorical domain
   */
  DiscreteDomain(bool is_inverted = false) : is_inverted_(is_inverted) {}

  std::string label(T value) const {
    return StringUtil::toString(value);
  }

  double scale(T value) const {
    size_t index = categories_.end() - std::find(
        categories_.begin(),
        categories_.end(),
        value);

    if (index < 1) {
      RAISE(kRuntimeError, "can't scale value");
    }

    double cardinality = (double) categories_.size();
    auto scaled = ((double) index - 0.5f) / cardinality;

    if (is_inverted_) {
      return 1.0 - scaled;
    } else {
      return scaled;
    }
  }

  std::pair<double, double> scaleRange(T value) const {
    size_t index = categories_.end() - std::find(
        categories_.begin(),
        categories_.end(),
        value);

    if (index < 1) {
      RAISE(kRuntimeError, "can't scale value");
    }

    auto cardinality = (double) categories_.size();
    auto begin = (double) (index - 1) / cardinality;;
    auto end = (double) index / cardinality;

    if (is_inverted_) {
      return std::make_pair(1.0 - begin, 1.0 - end);
    } else {
      return std::make_pair(begin, end);
    }
  }

  void addValue(const T& value) {
    addCategory(value);
  }

  void addCategory(const T& category) {
    bool insert = std::find(
        categories_.begin(),
        categories_.end(),
        category) == categories_.end();

    if (insert) {
      categories_.emplace_back(category);
    }
  }

  const std::vector<double> getTicks() const {
    std::vector<double> ticks{0.0};

    for (const auto category : categories_) {
      auto range = scaleRange(category);
      ticks.push_back(range.second);
    }

    return ticks;
  }

  const std::vector<std::pair<double, std::string>> getLabels() const {
    std::vector<std::pair<double, std::string>> labels;

    for (const auto category : categories_) {
      auto point = scale(category);
      labels.emplace_back(point, label(category));
    }

    return labels;
  }

  bool contains(T value) const {
    return std::find(
        categories_.begin(),
        categories_.end(),
        value) != categories_.end();
  }

  void setInverted(bool inverted) {
    is_inverted_ = inverted;
  }

  void build() {}

protected:
  bool is_inverted_;
  std::vector<T> categories_;
};

}
}
#endif
