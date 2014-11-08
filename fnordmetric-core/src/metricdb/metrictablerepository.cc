/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/metricdb/metrictablerepository.h>
#include <fnordmetric/metricdb/metrictableref.h>

namespace fnordmetric {
namespace metricdb {

MetricTableRepository::MetricTableRepository(
    IMetricRepository* metric_repo) :
    metric_repo_(metric_repo) {}

query::TableRef* MetricTableRepository::getTableRef(
    const std::string& table_name) const {
  auto metric = metric_repo_->findMetric(table_name);

  if (metric == nullptr) {
    return query::TableRepository::getTableRef(table_name);
  }

  return new MetricTableRef(metric);
}

}
}

