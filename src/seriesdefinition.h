/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
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
