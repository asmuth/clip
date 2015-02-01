/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <atomic>
#include <mutex>
#include <fnordmetric/metricdb/metric.h>
#include <fnordmetric/sql/backends/crate/crateconnection.h>

namespace fnordmetric {
namespace metricdb {
namespace crate_backend {

using fnord::util::DateTime;
using query::crate_backend::CrateConnection;

class Metric : public fnordmetric::metricdb::IMetric {
public:
  Metric(const std::string& key, CrateConnection connection);

  void scanSamples(
      const DateTime& time_begin,
      const DateTime& time_end,
      std::function<bool (Sample* sample)> callback) override;

  size_t totalBytes() const override;
  DateTime lastInsertTime() const override;
  std::set<std::string> labels() const override;
  bool hasLabel(const std::string& label) const override;
  size_t count(
    const DateTime& time_begin,
    const DateTime& time_end
  ) const;

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
