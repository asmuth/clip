/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_DOMAIN_H
#define _FNORDMETRIC_DOMAIN_H
#include <stdlib.h>
#include <assert.h>
#include "../format.h"

namespace fnordmetric {

class Domain {
public:

  Domain() : Domain(0, 0, false) {}

  Domain(
    double min_value,
    double max_value,
    bool is_logarithmic) :
    min_value_(min_value),
    max_value_(max_value),
    is_logarithmic_(is_logarithmic) {}

  double scale(double value) const {
    if (value <= min_value_) {
      return 0.0f;
    }

    if (value >= max_value_) {
      return 1.0f;
    }

    return (value - min_value_) / (max_value_ - min_value_);
  }

  double valueAt(double index) const {
    return min_value_ + (max_value_ - min_value_) * index;
  }

  std::string labelAt(double index) {
    return format::numberToHuman(valueAt(index));
  }

protected:
  double min_value_;
  double max_value_;
  bool is_logarithmic_;
};

}
#endif
