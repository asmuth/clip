/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord/base/exception.h>
#include <fnordmetric/sql/backends/metricservice/metrictablerepository.h>
#include <fnordmetric/sql/backends/metricservice/metrictableref.h>

namespace fnordmetric {
namespace query {

MetricTableRepository::MetricTableRepository(
    fnord::metric_service::IMetricRepository* metric_repo) :
    metric_repo_(metric_repo) {}

query::TableRef* MetricTableRepository::getTableRef(
    const std::string& table_name) const {
  auto metric = metric_repo_->findMetric(table_name);

  if (metric == nullptr) {
    auto tbl_ref = query::TableRepository::getTableRef(table_name);

    if (tbl_ref == nullptr) {
      RAISE(kRuntimeError, "unknown table");
    }

    return tbl_ref;
  }

  return new MetricTableRef(metric);
}

}
}

