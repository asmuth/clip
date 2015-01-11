/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_METRICDB_TIMESERIESQUERY_H_
#define _FNORDMETRIC_METRICDB_TIMESERIESQUERY_H_
#include <fnord/base/datetime.h>
#include <fnord/base/stdtypes.h>
#include <fnord/service/metric/sample.h>
#include <fnord/service/metric/metricservice.h>

namespace fnord {
namespace metric_service {

class TimeseriesQuery {
public:
  typedef Tuple<String, DateTime, double> ResultRowType;

  enum class AggregationFunction {
    kNoAggregation,
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

  TimeseriesQuery();

  String metric_key;
  AggregationFunction aggr_fn;
  Duration aggr_window;
  Duration aggr_step;
  double scale;

  std::vector<std::string> group_by;

  String join_metric_key;
  JoinFunction join_fn;
  AggregationFunction join_aggr_fn;

  void run(
      const DateTime& from,
      const DateTime& until,
      MetricService* metric_service,
      std::vector<ResultRowType>* out);

protected:
  struct Group {
    Vector<Pair<DateTime, double>> values;
    Vector<Pair<DateTime, double>> joined_values;
  };

  void processSample(Sample* sample, bool joined);

  void emitGroup(
      const String& group_name,
      Group* group,
      std::vector<ResultRowType>* out);

  void emitGroupWithoutAggregation(
      const String& group_name,
      Group* group,
      std::vector<ResultRowType>* out);

  void emitWindow(
      const String& group_name,
      DateTime window_time,
      Vector<Pair<DateTime, double>>::iterator values_begin,
      Vector<Pair<DateTime, double>>::iterator values_end,
      Vector<Pair<DateTime, double>>::iterator joined_values_begin,
      Vector<Pair<DateTime, double>>::iterator joined_values_end,
      std::vector<ResultRowType>* out);

  HashMap<String, Group> groups_;
};

}
}
#endif
