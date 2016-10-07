/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "metricd/metricservice.h"
#include "tsdb/TimeWindowPartitioner.h"
#include "fnord/wallclock.h"
#include "fnord/protobuf/msg.h"

namespace fnord {
namespace metric_service {

MetricService::MetricService(
      const String& tsdb_namespace,
      tsdb::TSDBClient* tsdb) :
      tsdb_namespace_(tsdb_namespace),
      tsdb_(tsdb) {}

std::vector<IMetric*> MetricService::listMetrics() const {
  RAISE(kNotYetImplementedError);
}

void MetricService::insertSample(
    const std::string& metric_key,
    double value,
    const std::vector<std::pair<std::string, std::string>>& labels) {
  auto sample_time = WallClock::unixMicros();

  metricd::MetricSample smpl;
  smpl.set_value(value);
  smpl.set_time(sample_time);

  for (const auto& l : labels){
    auto label = smpl.add_labels();
    label->set_key(l.first);
    label->set_value(l.second);
  }

  auto partition_key = tsdb::TimeWindowPartitioner::partitionKeyFor(
      metric_key,
      sample_time,
      600 * kMicrosPerSecond);

  auto record_id = rnd_.sha1();

  tsdb_->insertRecord(
      tsdb_namespace_,
      metric_key,
      partition_key,
      record_id,
      *msg::encode(smpl));
}

void MetricService::scanSamples(
    const std::string& metric_key,
    const fnord::DateTime& time_begin,
    const fnord::DateTime& time_end,
    std::function<void (const metricd::MetricSample& sample)> callback) {
  auto partitions = tsdb::TimeWindowPartitioner::partitionKeysFor(
      metric_key,
      time_begin,
      time_end,
      600 * kMicrosPerSecond);

  for (const auto& partition_key : partitions) {
    tsdb_->fetchPartition(
        tsdb_namespace_,
        metric_key,
        partition_key,
        [callback] (const Buffer& buf) {
      callback(msg::decode<metricd::MetricSample>(buf));
    });
  }
}

Sample MetricService::getMostRecentSample(const std::string& metric_key) {
  RAISE(kNotYetImplementedError);
}

} // namespace metric_service
} // namsepace fnord

