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
#include <fnordmetric/sql/runtime/tablescan.h>
#include <fnordmetric/sql/svalue.h>

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

  if (metric_->hasLabel(name)) {
    fields_.emplace_back(name);
    return fields_.size() + 1;
  }

  return -1;
}

void MetricTableRef::executeScan(query::TableScan* scan) {
  auto begin = fnord::util::DateTime::epoch();
  auto limit = fnord::util::DateTime::now();

  metric_->scanSamples(
      begin,
      limit,
      [this, scan] (MetricCursor* cursor) -> bool {
        auto sample = cursor->sample<double>();
        auto time = fnord::util::DateTime(cursor->time());

        std::vector<query::SValue> row;
        row.emplace_back(time);
        row.emplace_back(sample->value());

        // FIXPAUL slow!
        for (const auto& field : fields_) {
          bool found = false;

          for (const auto& label : sample->labels()) {
            if (label.first == field) {
              found = true;
              row.emplace_back(label.second);
              break;
            }
          }

          if (!found) {
            row.emplace_back();
          }
        }

        return scan->nextRow(row.data(), row.size());
      });
}


}
}

