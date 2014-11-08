/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_METRICDB_METRIC_H_
#define _FNORDMETRIC_METRICDB_METRIC_H_
#include <fnordmetric/metricdb/sample.h>
#include <fnordmetric/util/datetime.h>
#include <string>
#include <vector>
#include <set>

namespace fnordmetric {
namespace metricdb {

class Metric {
public:
  Metric(const std::string& key);
  virtual ~Metric();

  virtual void insertSample(
      double value,
      const std::vector<std::pair<std::string, std::string>>& labels) = 0;

  virtual void scanSamples(
      const fnord::util::DateTime& time_begin,
      const fnord::util::DateTime& time_end,
      std::function<bool (Sample* sample)> callback) = 0;

  const std::string& key() const;
  virtual size_t totalBytes() const = 0;
  virtual uint64_t lastInsertTime() const = 0;
  virtual std::set<std::string> labels() const = 0;
  virtual bool hasLabel(const std::string& label) const = 0;

protected:
  const std::string key_;
};

}
}
#endif
