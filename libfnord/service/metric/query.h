/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_METRICDB_QUERY_H_
#define _FNORDMETRIC_METRICDB_QUERY_H_
#include <fnord/base/datetime.h>
#include <fnord/base/stdtypes.h>
#include <fnord/service/metric/sample.h>
#include <fnord/service/metric/metricservice.h>

namespace fnord {
namespace metric_service {

class Query {
public:
  typedef Tuple<String, DateTime, double> ResultRowType;

  enum class AggregationFunction {
    kAggregateMax,
    kAggregateMin,
    kAggregateSum
  };

  enum class JoinFunction {
    kJoinMultiply,
    kJoinDivide
  };

  static AggregationFunction aggrFnFromString(const String& str);
  static JoinFunction joinFnFromString(const String& str);

  String metric_key;
  AggregationFunction aggr_fn;
  Duration aggr_window;
  Duration aggr_step;
  double scale;

  std::vector<std::string> group_by;

  String join_metric_key;
  JoinFunction join_fn;
  AggregationFunction join_aggr_fn;

  Query() :
    aggr_fn(AggregationFunction::kAggregateSum),
    aggr_window(60 * kMicrosPerSecond),
    aggr_step(10 * kMicrosPerSecond),
    scale(1.0),
    join_fn(JoinFunction::kJoinDivide),
    join_aggr_fn(AggregationFunction::kAggregateSum) {}

  void run(
      const DateTime& from,
      const DateTime& until,
      MetricService* metric_service,
      std::vector<ResultRowType>* out);

protected:

  void processSample(Sample* sample, bool joined);

  HashMap<String, double> groups_;
};

}
}
#endif
