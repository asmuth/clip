/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_METRICDB_METRICTABLEREF_H
#define _FNORDMETRIC_METRICDB_METRICTABLEREF_H
#include <fnord-metricdb/metric.h>
#include <fnordmetric/sql/backends/tableref.h>
#include <stdlib.h>
#include <string>
#include <memory>

namespace fnordmetric {
namespace query {

class MetricTableRef : public query::TableRef {
public:
  MetricTableRef(fnord::metric_service::IMetric* metric);

  int getColumnIndex(const std::string& name) override;
  std::string getColumnName(int index) override;
  void executeScan(query::TableScan* scan) override;
  std::vector<std::string> columns() override;

protected:
  fnord::metric_service::IMetric* metric_;
  std::vector<std::string> fields_;
};

}
}
#endif
