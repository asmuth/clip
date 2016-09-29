/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <metricd/backends/tsdb/metricrepository.h>

namespace fnord {
namespace metric_service {
namespace tsdb_backend {

MetricRepository::MetricRepository(
    const String& tsdb_prefix,
    tsdb::TSDBClient* tsdb) :
    tsdb_prefix_(tsdb_prefix),
    tsdb_(tsdb) {}

Metric* MetricRepository::createMetric(const std::string& key) {
  return new tsdb_backend::Metric(key, tsdb_prefix_, tsdb_);
}

}
}
}

