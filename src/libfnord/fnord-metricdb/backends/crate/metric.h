/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_METRICDB_BACKENDS_CRATE_METRIC_H_
#define _FNORD_METRICDB_BACKENDS_CRATE_METRIC_H_
#include <atomic>
#include <mutex>
#include <fnord-metricdb/metric.h>
#include <fnord-metricdb/backends/crate/crateconnection.h>

namespace fnord {
namespace metric_service {
namespace crate_backend {

using fnord::DateTime;

class Metric : public IMetric {
public:
  Metric(const std::string& key, CrateConnection connection);

  void scanSamples(
      const DateTime& time_begin,
      const DateTime& time_end,
      std::function<bool (Sample* sample)> callback) override;

  Sample getSample() override;

  size_t totalBytes() const override;
  DateTime lastInsertTime() const override;
  std::set<std::string> labels() const override;
  bool hasLabel(const std::string& label) const override;

protected:

  void insertSampleImpl(
      double value,
      const std::vector<std::pair<std::string, std::string>>& labels) override;

  std::set<std::string> labels_;
  CrateConnection connection_;
};

}
}
}
#endif
