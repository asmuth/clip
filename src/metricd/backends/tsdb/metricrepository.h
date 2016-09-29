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
#include <metricd/backends/tsdb/metric.h>
#include <metricd/metricrepository.h>

namespace fnord {
namespace metric_service {
namespace tsdb_backend {

class MetricRepository : public fnord::metric_service::IMetricRepository {
public:

  MetricRepository(
      const String& tsdb_prefix,
      tsdb::TSDBClient* tsdb);

  Metric* createMetric(const std::string& key) override;

protected:
  String tsdb_prefix_;
  tsdb::TSDBClient* tsdb_;
};

}
}
}
