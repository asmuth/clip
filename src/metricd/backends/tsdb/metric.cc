/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord/exception.h>
#include <fnord/wallclock.h>
#include <fnord/protobuf/msg.h>
#include <metricd/backends/tsdb/metric.h>
#include <metricd/Sample.pb.h>

namespace fnord {
namespace metric_service {
namespace tsdb_backend {

using fnord::DateTime;
using fnord::WallClock;

Metric::Metric(
    const String& key,
    const String& tsdb_prefix,
    tsdb::TSDBClient* tsdb) :
    IMetric(key),
    tsdb_prefix_(tsdb_prefix),
    tsdb_(tsdb),
    total_bytes_(0),
    last_insert_time_(0) {}

void Metric::insertSampleImpl(
    double value,
    const std::vector<std::pair<std::string, std::string>>& labels) {
  RAISE(kNotYetImplementedError);
}

void Metric::scanSamples(
    const DateTime& time_begin,
    const DateTime& time_end,
    std::function<bool (Sample* sample)> callback) {
  RAISE(kNotYetImplementedError);
}

Sample Metric::getSample() {
  RAISE(kNotYetImplementedError);
}

size_t Metric::totalBytes() const {
  return total_bytes_;
}

DateTime Metric::lastInsertTime() const {
  return DateTime(last_insert_time_);
}

std::set<std::string> Metric::labels() const {
  std::lock_guard<std::mutex> lock_holder(labels_mutex_);
  return labels_;
}

bool Metric::hasLabel(const std::string& label) const {
  std::lock_guard<std::mutex> lock_holder(labels_mutex_);
  return labels_.find(label) != labels_.end();
}

}
}
}

