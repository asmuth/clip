/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/metricdb/metrictableref.h>

namespace fnordmetric {
namespace query {
class TableScan;
}

namespace metricdb {

MetricTableRef::MetricTableRef(Metric* metric) : metric_(metric) {}

int MetricTableRef::getColumnIndex(const std::string& name) {
  if (name == "time") {
    return 0;
  }

  if (name == "value") {
    return 1;
  }


  return -1;
}

void MetricTableRef::executeScan(query::TableScan* scan) {

}

}
}

