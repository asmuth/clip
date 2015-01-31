/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord-metricdb/backends/inmemory/metricrepository.h>

namespace fnord {
namespace metric_service {
namespace inmemory_backend {

Metric* MetricRepository::createMetric(const std::string& key) {
  return new inmemory_backend::Metric(key);
}

}
}
}

