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
#include <fnordmetric/metricdb/compactionpolicy.h>
#include <fnordmetric/metricdb/labelindex.h>
#include <fnordmetric/metricdb/metriccursor.h>
#include <fnordmetric/metricdb/metricsnapshot.h>
#include <fnordmetric/metricdb/sample.h>
#include <fnordmetric/metricdb/samplereader.h>
#include <fnordmetric/metricdb/tokenindex.h>
#include <fnordmetric/sstable/livesstable.h>
#include <fnordmetric/util/datetime.h>
#include <string>
#include <vector>

using namespace fnord;
namespace fnordmetric {
namespace metricdb {

class Metric {
public:
  static constexpr const size_t kLiveTableMaxSize = 2 << 19; /* 1MB */
  static constexpr const uint64_t kLiveTableIdleTimeMicros = 
      5 * 60 * 1000000; /* 5 minutes */

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

  void compact(CompactionPolicy* compaction = nullptr);

  void setLiveTableMaxSize(size_t max_size);
  void setLiveTableIdleTimeMicros(uint64_t idle_time_micros);

  size_t numTables() const;
  size_t totalBytes() const;
  uint64_t lastInsertTime() const;
  std::set<std::string> labels() const;
  bool hasLabel(const std::string& label) const;

protected:
  std::shared_ptr<MetricSnapshot> getSnapshot() const;
  std::shared_ptr<MetricSnapshot> getOrCreateSnapshot();
  std::shared_ptr<MetricSnapshot> createSnapshot(bool writable);

  const std::string key_;
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
#endif
