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
#include <fnord/service/metric/query.h>

namespace fnord {
namespace metric_service {

Query::AggregationFunction Query::aggrFnFromString(const String& str) {
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

Query::JoinFunction Query::joinFnFromString(const String& str) {
  if (str == "divide") {
    return JoinFunction::kJoinDivide;
  }

  if (str == "multiply") {
    return JoinFunction::kJoinMultiply;
  }

  RAISEF(kIllegalArgumentError, "invalid join function: $0", str);
}

void Query::run(
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
}

void Query::processSample(Sample* sample, bool joined) {
  fnord::iputs("sample... $0, $1, $2", sample->time(), sample->value(), sample->labels());
}

}
}
