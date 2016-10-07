/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_METRICDB_METRICREPOSITORY_H_
#define _FNORDMETRIC_METRICDB_METRICREPOSITORY_H_
#include <metricd/metric.h>
#include <mutex>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

using namespace fnord;
namespace fnord {
namespace metric_service {

class IMetricRepository {
public:
  virtual ~IMetricRepository() {}
  IMetric* findMetric(const std::string& key) const;
  IMetric* findOrCreateMetric(const std::string& key);
  std::vector<IMetric*> listMetrics() const;
protected:
  virtual IMetric* createMetric(const std::string& key) = 0;
  std::unordered_map<std::string, std::unique_ptr<IMetric>> metrics_;
  mutable std::mutex metrics_mutex_;
};

}
}
#endif
