/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <metricd/metricdb/sample.h>

using fnord::util::DateTime;

namespace fnordmetric {
namespace metricdb {

Sample::Sample(
    const DateTime& time,
    double value,
    const std::vector<std::pair<std::string, std::string>>& labels) :
    time_(time),
    value_(value),
    labels_(labels) {}

const DateTime& Sample::time() {
  return time_;
}

double Sample::value() {
  return value_;
}

const std::vector<std::pair<std::string, std::string>>& Sample::labels() {
  return labels_;
}


}
}

