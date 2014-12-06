/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_METRICDB_DISK_BACKEND_METRIC_H_
#define _FNORDMETRIC_METRICDB_DISK_BACKEND_METRIC_H_
#include <string>
#include <vector>
#include <fnord/base/datetime.h>
#include <fnord/io/filerepository.h>
#include <fnord/service/metric/backends/disk/compactionpolicy.h>
#include <fnord/service/metric/backends/disk/labelindex.h>
#include <fnord/service/metric/backends/disk/metriccursor.h>
#include <fnord/service/metric/backends/disk/metricsnapshot.h>
#include <fnord/service/metric/backends/disk/samplereader.h>
#include <fnord/service/metric/backends/disk/tokenindex.h>
#include <fnord/service/metric/metric.h>
#include <fnord/service/metric/sample.h>

using namespace fnord;
namespace fnord {
namespace metric_service {
namespace disk_backend {

class Metric : public fnord::metric_service::IMetric {
public:
  static constexpr const size_t kLiveTableMaxSize = 2 << 19; /* 1MB */
  static constexpr const uint64_t kLiveTableIdleTimeMicros = 
      5 * 60 * 1000000; /* 5 minutes */

  Metric(const std::string& key, io::FileRepository* file_repo);

  Metric(
      const std::string& key,
      io::FileRepository* file_repo,
      std::vector<std::unique_ptr<TableRef>>&& tables);

  void scanSamples(
      const fnord::DateTime& time_begin,
      const fnord::DateTime& time_end,
      std::function<bool (Sample* sample)> callback) override;

  void compact(CompactionPolicy* compaction = nullptr);

  void setLiveTableMaxSize(size_t max_size);
  void setLiveTableIdleTimeMicros(uint64_t idle_time_micros);
  size_t numTables() const;

  size_t totalBytes() const override;
  DateTime lastInsertTime() const override;
  std::set<std::string> labels() const override;
  bool hasLabel(const std::string& label) const override;

protected:
  void insertSampleImpl(
      double value,
      const std::vector<std::pair<std::string, std::string>>& labels) override;

  std::shared_ptr<MetricSnapshot> getSnapshot() const;
  std::shared_ptr<MetricSnapshot> getOrCreateSnapshot();
  std::shared_ptr<MetricSnapshot> createSnapshot(bool writable);

  io::FileRepository const* file_repo_;
  std::shared_ptr<MetricSnapshot> head_;
  mutable std::mutex head_mutex_;
  std::mutex append_mutex_;
  std::mutex compaction_mutex_;
  uint64_t max_generation_;
  TokenIndex token_index_;
  LabelIndex label_index_;

  size_t live_table_max_size_; // FIXPAUL make atomic
  uint64_t live_table_idle_time_micros_; // FIXPAUL make atomic
  uint64_t last_insert_; // FIXPAUL make atomic
};

}
}
}
#endif
