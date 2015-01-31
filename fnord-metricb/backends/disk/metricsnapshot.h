/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_METRICDB_METRICSNAPSHOT_H_
#define _FNORDMETRIC_METRICDB_METRICSNAPSHOT_H_
#include <fnord/service/metric/backends/disk/tableref.h>
#include <string>
#include <vector>

using namespace fnord;
namespace fnord {
namespace metric_service {
namespace disk_backend {

class MetricSnapshot {
public:
  explicit MetricSnapshot();

  MetricSnapshot(const MetricSnapshot& other);
  MetricSnapshot& operator=(const MetricSnapshot& other) = delete;

  void appendTable(std::shared_ptr<TableRef> table);
  const std::vector<std::shared_ptr<TableRef>>& tables() const;
  std::shared_ptr<MetricSnapshot> clone() const;

  void setWritable(bool writable);
  bool isWritable() const;

protected:
  explicit MetricSnapshot(
      const std::vector<std::shared_ptr<TableRef>>& tables);

  std::vector<std::shared_ptr<TableRef>> tables_;
  bool writable_;
};

}
}
}
#endif
