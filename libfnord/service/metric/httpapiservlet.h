/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_METRIC_SERVICE_HTTPAPISERVLET_H
#define _FNORD_METRIC_SERVICE_HTTPAPISERVLET_H
#include "fnord/chart/areachart.h"
#include "fnord/chart/canvas.h"
#include "fnord/chart/linechart.h"
#include "fnord/chart/svgtarget.h"
#include "fnord/net/http/httpservice.h"
#include "fnord/service/metric/metricservice.h"
#include "fnord/service/metric/timeseriesquery.h"
#include "fnord/json/json.h"

namespace fnord {
namespace metric_service {

class HTTPAPIServlet : public fnord::http::HTTPService {
public:
  enum class ResponseFormat {
    kJSON,
    kCSV,
    kSVG
  };

  HTTPAPIServlet(MetricService* metric_service);

  void handleHTTPRequest(
      fnord::http::HTTPRequest* req,
      fnord::http::HTTPResponse* res);

protected:

  void insertSample(
      http::HTTPRequest* request,
      http::HTTPResponse* response,
      URI* uri);

  void listMetrics(
      http::HTTPRequest* request,
      http::HTTPResponse* response,
      URI* uri);

  void timeseriesQuery(
      http::HTTPRequest* request,
      http::HTTPResponse* response,
      URI* uri);

  void renderMetricJSON(
      fnord::metric_service::IMetric* metric,
      json::JSONOutputStream* json) const;

  template <class ChartType>
  void renderCharts(
      chart::Canvas* canvas,
      const String& style,
      Vector<ScopedPtr<TimeseriesQuery>>::iterator* begin,
      Vector<ScopedPtr<TimeseriesQuery>>::iterator end,
      Vector<chart::Drawable*>* charts);

  void applyChartStyles(
      const URI::ParamList& params,
      const DateTime& from,
      const DateTime& until,
      Vector<chart::Drawable*>* charts);

  ResponseFormat formatFromString(const String& format);

  MetricService* metric_service_;
};


template <class ChartType>
void HTTPAPIServlet::renderCharts(
    chart::Canvas* canvas,
    const String& style,
    Vector<ScopedPtr<TimeseriesQuery>>::iterator* qbegin,
    Vector<ScopedPtr<TimeseriesQuery>>::iterator qend,
    Vector<chart::Drawable*>* charts) {
  auto chart = canvas->addChart<ChartType>();
  charts->push_back(chart);

  for (; *qbegin != qend && (**qbegin)->draw_style == style; ++(*qbegin)) {
    Vector<chart::Series2D<DateTime, double>*> res_series;
    (**qbegin)->renderSeries(&res_series);
    for (auto& series : res_series) {
      chart->addSeries(series);
    }
  }
}

}
}
#endif
