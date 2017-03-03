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
#include <fnordmetric/sample.h>

namespace fnordmetric {

Sample::Sample(
    const std::string& metric_name,
    const std::string& value,
    TimestampType time,
    const LabelSet& labels) :
    metric_name_(metric_name),
    value_(value),
    time_(time),
    labels_(labels) {}

const std::string& Sample::getMetricName() const {
  return metric_name_;
}

const std::string& Sample::getValue() const {
  return value_;
}

TimestampType Sample::getTime() const {
  return time_;
}

const LabelSet& Sample::getLabels() const {
  return labels_;
}

} // namespace fnordmetric

