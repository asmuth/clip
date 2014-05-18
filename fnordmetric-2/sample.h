/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_SAMPLE_H
#define _FNORDMETRIC_SAMPLE_H

#include "dimension.h"

namespace fnordmetric {

/**
 * Sample
 */
class Sample {
public:

  Sample() {}
  Sample(const Sample& copy) = delete;

  template <typename... D>
  void addValues(int64_t value, D... tail) {
    printf("push int value\n");
    addValues(tail...);
  }

protected:

  void addValues() {}
  std::vector<uint8_t> data_;

};

}

#endif
