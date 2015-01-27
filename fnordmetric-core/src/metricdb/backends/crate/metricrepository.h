/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/metricdb/backends/crate/metric.h>
#include <fnordmetric/metricdb/metricrepository.h>
#include <fnordmetric/sql/backends/crate/crateconnection.h>

namespace fnordmetric {
namespace metricdb {
namespace crate_backend {

class MetricRepository : public fnordmetric::metricdb::IMetricRepository {
public:
    MetricRepository(std::string host);
protected:
    Metric * createMetric(const std::string& key) override;
    CrateConnection connection_;
};

}
}
}
