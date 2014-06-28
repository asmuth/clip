/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_SERIESDEFINITION_H
#define _FNORDMETRIC_SERIESDEFINITION_H
#include <stdlib.h>
#include "query/svalue.h"

namespace fnordmetric {

class SeriesDefinition {
public:
  SeriesDefinition(const std::string& name) :
      name_(name) {}

  void addDatum(const std::vector<query::SValue>&& datum) {
    data_.emplace_back(std::move(datum));
  }

  const std::vector<std::vector<query::SValue>>& getData() {
    return data_;
  }

protected:
  std::vector<std::vector<query::SValue>> data_;
  const std::string name_;
};

}
#endif
