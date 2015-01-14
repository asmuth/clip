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
#include <fnord/chart/series.h>
#include <fnord/service/metric/sample.h>
#include <fnord/service/metric/metricservice.h>

namespace fnord {
namespace metric_service {

class TimeseriesQuery {
public:
  enum class AggregationFunction {
    kNoAggregation,
    kAggregateMax,
    kAggregateMin,
    kAggregateSum
  };

  enum class JoinFunction {
    kNoJoin,
    kJoinMultiply,
    kJoinDivide
  };

  static AggregationFunction aggrFnFromString(const String& str);
  static JoinFunction joinFnFromString(const String& str);

  TimeseriesQuery();

  String metric_key;
  String draw_style;
  String color;
  String line_style;
  String line_width;
  String point_style;
  String point_size;
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
      MetricService* metric_service);

  void renderCSV(Buffer* out);

  void renderSeries(Vector<chart::Series2D<DateTime, double>*>* out);

protected:
  typedef Tuple<String, DateTime, double> ResultRowType;

  struct Group {
    Vector<Pair<DateTime, double>> values;
    Vector<Pair<DateTime, double>> joined_values;
  };

  void processSample(Sample* sample, bool joined);

  void emitGroup(
      const String& group_name,
      Group* group);

  void emitGroupWithoutAggregation(
      const String& group_name,
      Group* group);

  void emitWindow(
      const String& group_name,
      DateTime window_time,
      Vector<Pair<DateTime, double>>::iterator values_begin,
      Vector<Pair<DateTime, double>>::iterator values_end,
      Vector<Pair<DateTime, double>>::iterator joined_values_begin,
      Vector<Pair<DateTime, double>>::iterator joined_values_end);

  double aggregateWindow(
      AggregationFunction aggr,
      Vector<Pair<DateTime, double>>::iterator values_begin,
      Vector<Pair<DateTime, double>>::iterator values_end) const;

  double aggregateWindowMax(
      Vector<Pair<DateTime, double>>::iterator values_begin,
      Vector<Pair<DateTime, double>>::iterator values_end) const;

  double aggregateWindowMin(
      Vector<Pair<DateTime, double>>::iterator values_begin,
      Vector<Pair<DateTime, double>>::iterator values_end) const;

  double aggregateWindowSum(
      Vector<Pair<DateTime, double>>::iterator values_begin,
      Vector<Pair<DateTime, double>>::iterator values_end) const;

  String fullGroupName(const String& group_name) const;

  HashMap<String, Group> groups_;
  Vector<ResultRowType> results_;
  DateTime from_;
  DateTime until_;
};

}
}
#endif
