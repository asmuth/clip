/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord-metricdb/backends/crate/metric.h>
#include <fnord-base/wallclock.h>

using namespace fnord;

namespace fnord {
namespace metric_service {
namespace crate_backend {

Metric::Metric(
    const std::string& key,
    CrateConnection connection) :
    connection_(connection),
    IMetric(key) {
  labels_ = labels();
}

void Metric::insertSampleImpl(
    double value,
    const std::vector<std::pair<std::string, std::string>>& labels) {
  auto now = WallClock::unixMicros() / 1000;

  std::vector<std::string> args;
  args.emplace_back(StringUtil::toString(now));
  args.emplace_back(StringUtil::toString(value));

  std::string insert = "INSERT INTO metrics.";
  insert.append(key_);

  std::string keys = "(time, value";
  std::string values = " VALUES (?, ?";

  for (const auto& lbl_pair : labels) {
    keys.append(",");
    keys.append(lbl_pair.first);
    values.append(",?");
    args.emplace_back(lbl_pair.second);
    if(!hasLabel(lbl_pair.first.c_str())){
      labels_.emplace(lbl_pair.first.c_str());
    }
  }

  keys.append(")");
  values.append(")");
  insert.append(keys);
  insert.append(values);

  connection_.executeQuery(insert, args);
}

void Metric::scanSamples(
    const DateTime& time_begin,
    const DateTime& time_end,
    std::function<bool (Sample* sample)> callback) {
  std::string sample_q = "select time, value";
  for(const auto& col : labels_){
      sample_q.append(", ");
      sample_q.append(col);
  }

  sample_q.append(" from metrics.");
  sample_q.append(key_);
  sample_q.append(" where time >= ? and time <= ? order by time asc");

  std::vector<std::string> args;
  args.emplace_back(StringUtil::toString(time_begin));
  args.emplace_back(StringUtil::toString(time_end));

  connection_.executeQuery(
      sample_q,
      args,
      [this, callback] (const json::JSONObject& row) -> bool {
        // convert from milliseconds to microseconds
        //uint64_t time = row[0].GetUint64() * 1000;
        //double value = row[1].GetDouble();
        //int j = 2;
        //std::vector<std::pair<std::string, std::string>> labels;
        //for( auto col : labels_) {
        //  if(row.Size() > j && !row[j].IsNull()){
        //     if(row[j].IsString()) {
        //      std::string label_str_val = row[j].GetString();
        //      labels.emplace_back(std::make_pair(col, label_str_val));
        //     }
        //  }
        //  j++;
        //}
        //Sample cb_sample(time, value, labels);
        //if(!callback(&cb_sample)){
        //    return false;
        //}
        return true;
      });
}

Sample Metric::getSample() {
  RAISE(kNotYetImplementedError);
}

size_t Metric::totalBytes() const {
  std::string size_q =
      "select cast(sum(size) as long) from sys.shards where table_name = ? " \
      "and schema_name = 'metrics'";

  std::vector<std::string> args;
  args.emplace_back(key_);

  size_t size = 0;
  connection_.executeQuery(
      size_q,
      args,
      [this, &size] (const json::JSONObject& row) -> bool {
        //if(row.Size() < 1 || !row[0].IsUint64()){
        //    return false;
        //}

        //size = row[0].GetUint64();
        //return true;
      });

  return size;
}

DateTime Metric::lastInsertTime() const {
  std::string last_ts = "select time from metrics.";
  last_ts.append(key_);
  last_ts.append(" order by time desc limit 1");

  uint64_t timestamp = 0;

  connection_.executeQuery(
      last_ts,
      [this, &timestamp] (const json::JSONObject& row) -> bool {
        //if(row.Size() < 1 || !row[0].IsUint64()){
        //    return false;
        //}
        //timestamp = row[0].GetUint64() * 1000;
        //return true;
      });

  return timestamp;
}

std::set<std::string> Metric::labels() const {
  std::string labels_q =
      "select column_name from information_schema.columns where " \
      "schema_name = 'metrics' and table_name = ? and column_name not " \
      "in ('time', 'value')";

  std::vector<std::string> args;
  std::set<std::string> labels;
  args.emplace_back(key_);

  connection_.executeQuery(
      labels_q,
      args,
      [this, &labels] (const json::JSONObject& row) -> bool {
        //if(row.Size() < 1 || !row[0].IsString()){
        //    return false;
        //}
        //std::string label = row[0].GetString();
        //labels.emplace(label);
        //return true;
      });

  return labels;
}

bool Metric::hasLabel(const std::string& label) const {
  return labels_.find(label) != labels_.end();
}

}
}
}
