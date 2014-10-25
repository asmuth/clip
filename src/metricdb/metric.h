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
#include <fnordmetric/io/filerepository.h>
#include <fnordmetric/metricdb/metriccursor.h>
#include <fnordmetric/metricdb/metricsnapshot.h>
#include <fnordmetric/metricdb/sample.h>
#include <fnordmetric/metricdb/samplereader.h>
#include <fnordmetric/sstable/livesstable.h>
#include <fnordmetric/util/datetime.h>
#include <string>
#include <vector>

using namespace fnord;
namespace fnordmetric {
namespace metricdb {

class Metric {
public:
  Metric(const std::string& key, io::FileRepository* file_repo);

  Metric(
      const std::string& key,
      io::FileRepository* file_repo,
      std::vector<std::unique_ptr<TableRef>>&& tables);

  void addSample(const Sample<double>& sample);

  void scanSamples(
      const fnord::util::DateTime& time_begin,
      const fnord::util::DateTime& time_end,
      std::function<bool (MetricCursor*)> callback);

  std::unique_ptr<MetricCursor> cursor() const;
  const std::string& key() const;

protected:
  std::shared_ptr<MetricSnapshot> getSnapshot() const;
  std::shared_ptr<MetricSnapshot> getOrCreateSnapshot();
  std::shared_ptr<MetricSnapshot> createSnapshot();

  const std::string key_;
  io::FileRepository const* file_repo_;
  std::shared_ptr<MetricSnapshot> head_;
  mutable std::mutex head_mutex_;
  std::mutex append_mutex_;
};

}
}
#endif
