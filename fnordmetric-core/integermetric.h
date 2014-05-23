/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_METRIC_H
#define _FNORDMETRIC_METRIC_H

#include <stdlib.h>
#include <stdint.h>
#include "imetric.h"

/**
 * Integer Metric
 *
 * see imetric.h for usage
 */
template<typename... D>
class IntegerMetric : public IMetric<uint64_t, D> {
public:

  virtual void sample(uint64_t value, D... dimensions) override {

  }

};



#endif
