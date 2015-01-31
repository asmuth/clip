/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord/service/metric/metric.h>
#include <fnord-base/exception.h>

namespace fnord {
namespace metric_service {

IMetric::IMetric(const std::string& key) : key_(key) {}
IMetric::~IMetric() {}

void IMetric::insertSample(
    double value,
    const std::vector<std::pair<std::string, std::string>>& labels) {
  // FIXPAUL slow slow slow!
  for (int i1 = 0; i1 < labels.size(); ++i1) {
    for (int i2 = 0; i2 < labels.size(); ++i2) {
      if (i1 != i2 && labels[i1].first == labels[i2].first) {
        RAISE(
            kIllegalArgumentError,
            "duplicate label: %s",
            labels[i1].first.c_str());
      }
    }
  }

  insertSampleImpl(value, labels);
}

const std::string& IMetric::key() const {
  return key_;
}

}
}

