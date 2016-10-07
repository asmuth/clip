/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <stdlib.h>
#include <string>
#include <vector>

namespace fnordmetric {

class Sample {
public:

  using TimestampType = uint64_t;

  Sample(
      TimestampType time,
      double value,
      const std::vector<std::pair<std::string, std::string>>& labels);

  TimestampType time();
  double value();

protected:
  const TimestampType& time_;
  double value_;
};

} // namespace fnordmetric

