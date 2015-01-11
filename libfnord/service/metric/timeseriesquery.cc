/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord/base/exception.h>
#include <fnord/base/inspect.h>
#include <fnord/service/metric/timeseriesquery.h>

namespace fnord {
namespace metric_service {

TimeseriesQuery::TimeseriesQuery() :
    aggr_fn(AggregationFunction::kNoAggregation),
    aggr_window(60 * kMicrosPerSecond),
    aggr_step(10 * kMicrosPerSecond),
    scale(1.0),
    join_fn(JoinFunction::kJoinDivide),
    join_aggr_fn(AggregationFunction::kAggregateSum) {}


TimeseriesQuery::AggregationFunction TimeseriesQuery::aggrFnFromString(const String& str) {
  if (str == "max") {
    return AggregationFunction::kAggregateMax;
  }

  if (str == "min") {
    return AggregationFunction::kAggregateMin;
  }

  if (str == "sum") {
    return AggregationFunction::kAggregateSum;
  }

  RAISEF(kIllegalArgumentError, "invalid aggregation function: $0", str);
}

TimeseriesQuery::JoinFunction TimeseriesQuery::joinFnFromString(const String& str) {
  if (str == "divide") {
    return JoinFunction::kJoinDivide;
  }

  if (str == "multiply") {
    return JoinFunction::kJoinMultiply;
  }

  RAISEF(kIllegalArgumentError, "invalid join function: $0", str);
}

void TimeseriesQuery::run(
    const DateTime& from,
    const DateTime& until,
    MetricService* metric_service,
    std::vector<ResultRowType>* out) {

  metric_service->scanSamples(
      metric_key,
      from,
      until,
      [this] (Sample* sample) -> bool {
        processSample(sample, false);
        return true;
      });

  if (join_metric_key.length() > 0) {
    metric_service->scanSamples(
        join_metric_key,
        from,
        until,
        [this] (Sample* sample) -> bool {
          processSample(sample, true);
          return true;
        });
  }

  if (aggr_fn == AggregationFunction::kNoAggregation) {
    for (auto& group : groups_) {
      emitGroupWithoutAggregation(group.first, &group.second, out);
    }
  } else {
    for (auto& group : groups_) {
      emitGroup(group.first, &group.second, out);
    }
  }
}

void TimeseriesQuery::processSample(Sample* sample, bool joined) {
  Vector<String> group_keyv;

  for (const auto& group : group_by) {
    bool found = false;

    for (const auto& label : sample->labels()) {
      if (label.first == group) {
        found = true;
        group_keyv.emplace_back(label.second);
        break;
      }
    }

    if (!found) {
      group_keyv.emplace_back("NULL");
    }
  }

  Group* group = nullptr;
  auto group_key = StringUtil::join(group_keyv, ", ");
  auto group_iter = groups_.find(group_key);
  if (group_iter == groups_.end()) {
    group = &groups_[group_key];
  } else {
    group = &group_iter->second;
  }

  if (joined) {
    group->joined_values.emplace_back(sample->time(), sample->value());
  } else {
    group->values.emplace_back(sample->time(), sample->value());
  }
}

void TimeseriesQuery::emitGroupWithoutAggregation(
    const String& group_name,
    Group* group,
    std::vector<ResultRowType>* out) {
  for (const auto& value : group->values) {
    out->emplace_back(group_name, value.first, value.second);
  }
}

void TimeseriesQuery::emitGroup(
    const String& group_name,
    Group* group,
    std::vector<ResultRowType>* out) {
  auto& values = group->values;
  auto& joined_values = group->joined_values;

  if (values.size() == 0) {
    return;
  }

  /* sort rows */
  auto cmp = [] (
      const Pair<DateTime, double>& a,
      const Pair<DateTime, double>& b) {
    return a.first.unixMicros() < b.first.unixMicros();
  };

  std::sort(values.begin(), values.end(), cmp);
  std::sort(joined_values.begin(), joined_values.end(), cmp);

  size_t window_start_idx = 0;
  size_t window_end_idx;
  uint64_t window_start_time = values[0].first.unixMicros();
  size_t joined_window_start_idx = 0;
  size_t joined_window_end_idx = 0;

  do {
    /* search end of current window */
    auto window_end_time = window_start_time + aggr_window.microseconds();
    for (
        window_end_idx = window_start_idx;
        window_end_idx < values.size() &&
            values[window_end_idx].first.unixMicros() < window_end_time;
        ++window_end_idx);

    /* find joined window */
    while (joined_window_start_idx < joined_values.size() &&
        joined_values[joined_window_start_idx].first.unixMicros() <
            window_start_time) {
      ++joined_window_start_idx;
    }

    for (
        joined_window_end_idx = joined_window_start_idx;
        joined_window_end_idx < joined_values.size() &&
            joined_values[joined_window_end_idx].first.unixMicros() <
                window_end_time;
        ++joined_window_end_idx);

    emitWindow(
        group_name,
        DateTime(window_end_time),
        values.begin() + window_start_idx,
        values.begin() + window_end_idx,
        joined_values.begin() + joined_window_start_idx,
        joined_values.begin() + joined_window_end_idx,
        out);

    /* advance window */
    window_start_time += aggr_step.microseconds();
    while (window_start_idx <= window_end_idx &&
        values[window_start_idx].first.unixMicros() < window_start_time) {
      window_start_idx++;
    }

  } while (window_end_idx < values.size());
}

void TimeseriesQuery::emitWindow(
    const String& group_name,
    DateTime window_time,
    Vector<Pair<DateTime, double>>::iterator values_begin,
    Vector<Pair<DateTime, double>>::iterator values_end,
    Vector<Pair<DateTime, double>>::iterator joined_values_begin,
    Vector<Pair<DateTime, double>>::iterator joined_values_end,
    std::vector<ResultRowType>* out) {
  fnord::iputs("emit window: $0 $1 $2 $3", group_name, window_time, Vector<Pair<DateTime, double>>(values_begin, values_end), Vector<Pair<DateTime, double>>(joined_values_begin, joined_values_end));
}

}
}
