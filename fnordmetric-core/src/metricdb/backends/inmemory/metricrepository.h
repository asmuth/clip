/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_METRICDB_INMEMORY_BACKEND_METRICREPOSITORY_H_
#define _FNORDMETRIC_METRICDB_INMEMORY_BACKEND_METRICREPOSITORY_H_
#include <metricd/metricdb/backends/inmemory/metric.h>
#include <metricd/metricdb/metricrepository.h>

namespace fnordmetric {
namespace metricdb {
namespace inmemory_backend {

class MetricRepository : public fnordmetric::metricdb::IMetricRepository {
protected:
  Metric* createMetric(const std::string& key) override;
};

}
}
}
#endif
