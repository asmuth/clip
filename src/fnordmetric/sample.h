/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <stdlib.h>
#include <string>
#include <map>

namespace fnordmetric {

using TimestampType = uint64_t;
using LabelSet = std::map<std::string, std::string>; // FIXME should be a flat map

class Sample {
public:

  Sample(
      const std::string& metric_name,
      const std::string& value,
      TimestampType time,
      const LabelSet& labels);

  const std::string& getMetricName() const;
  const std::string& getValue() const;
  TimestampType getTime() const;
  const LabelSet& getLabels() const;

protected:
  const std::string metric_name_;
  const std::string& value_;
  const TimestampType time_;
  const LabelSet labels_;
};

} // namespace fnordmetric

