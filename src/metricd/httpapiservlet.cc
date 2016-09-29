/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "fnord/wallclock.h"
#include "metricd/httpapiservlet.h"
#include "fnord/charts/axisdefinition.h"

namespace fnord {
namespace metric_service {

HTTPAPIServlet::HTTPAPIServlet(
    MetricService* metric_service) :
    metric_service_(metric_service) {}

void HTTPAPIServlet::handleHTTPRequest(
    fnord::http::HTTPRequest* req,
    fnord::http::HTTPResponse* res) {
  URI uri(req->uri());

  res->addHeader("Access-Control-Allow-Origin", "*");

  try {
    if (StringUtil::endsWith(uri.path(), "/insert")) {
      return insertSample(req, res, &uri);
    }

    if (StringUtil::endsWith(uri.path(), "/list")) {
      return listMetrics(req, res, &uri);
    }

    if (StringUtil::endsWith(uri.path(), "/timeseries")) {
      return timeseriesQuery(req, res, &uri);
    }

    if (StringUtil::endsWith(uri.path(), "/value")) {
      return valueQuery(req, res, &uri);
    }

    if (StringUtil::endsWith(uri.path(), "/histogram")) {
      //return timeseriesQuery(req, res, &uri);
    }

    if (StringUtil::endsWith(uri.path(), "/rollup")) {
      //return timeseriesQuery(req, res, &uri);
    }

    res->setStatus(fnord::http::kStatusNotFound);
    res->addBody("not found");
  } catch (const Exception& e) {
    res->setStatus(http::kStatusInternalServerError);
    res->addBody(StringUtil::format("error: $0", e.getMessage()));
  }
}

void HTTPAPIServlet::listMetrics(
    http::HTTPRequest* request,
    http::HTTPResponse* response,
    URI* uri) {
  response->setStatus(http::kStatusOK);
  response->addHeader("Content-Type", "application/json; charset=utf-8");
  json::JSONOutputStream jsons(response->getBodyOutputStream());

  jsons.beginObject();
  jsons.addObjectEntry("metrics");
  jsons.beginArray();

  fnord::URI::ParamList params = uri->queryParams();
  std::string filter_query;
  auto filter_enabled = fnord::URI::getParam(params, "filter", &filter_query);

  int limit = -1;
  std::string limit_string;
  if (fnord::URI::getParam(params, "limit", &limit_string)) {
    limit = std::stoi(limit_string);
  }

  int i = 0;
  for (const auto& metric : metric_service_->listMetrics()) {
    if (filter_enabled &&
        metric->key().find(filter_query) == std::string::npos) {
      continue;
    }

    if (i++ > 0) {
      jsons.addComma();
    }

    renderMetricJSON(metric, &jsons);

    if (limit > 0 && i == limit) {
      break;
    }
  }

  jsons.endArray();
  jsons.endObject();
}

void HTTPAPIServlet::insertSample(
    http::HTTPRequest* request,
    http::HTTPResponse* response,
    URI* uri) {
  const auto& postbody = request->body();
  fnord::URI::ParamList params;

  if (postbody.size() > 0) {
    fnord::URI::parseQueryString(postbody.toString(), &params);
  } else {
    params = uri->queryParams();
  }

  std::string metric_key;
  if (!fnord::URI::getParam(params, "metric", &metric_key)) {
    response->addBody("error: invalid metric key: " + metric_key);
    response->setStatus(http::kStatusBadRequest);
    return;
  }

  std::string value_str;
  if (!fnord::URI::getParam(params, "value", &value_str)) {
    response->addBody("error: missing ?value=... parameter");
    response->setStatus(http::kStatusBadRequest);
    return;
  }

  std::vector<std::pair<std::string, std::string>> labels;
  static const char kLabelParamPrefix[] = "label[";
  for (const auto& param : params) {
    const auto& key = param.first;
    const auto& value = param.second;

    if (key.compare(0, sizeof(kLabelParamPrefix) - 1, kLabelParamPrefix) == 0 &&
        key.back() == ']') {
      auto label_key = key.substr(
          sizeof(kLabelParamPrefix) - 1,
          key.size() - sizeof(kLabelParamPrefix));

      labels.emplace_back(label_key, value);
    }
  }

  double sample_value;
  try {
    sample_value = std::stod(value_str);
  } catch (std::exception& e) {
    response->addBody("error: invalid value: " + value_str);
    response->setStatus(http::kStatusBadRequest);
    return;
  }

  metric_service_->insertSample(metric_key, sample_value, labels);
  response->setStatus(http::kStatusCreated);
  response->addBody("ok");
}

void HTTPAPIServlet::timeseriesQuery(
    http::HTTPRequest* request,
    http::HTTPResponse* response,
    URI* uri) {
  auto params = uri->queryParams();

  auto resp_format = ResponseFormat::kCSV;
  DateTime from(WallClock::unixMicros() - kMillisPerHour);
  DateTime until;

  /* parse query param string */
  Vector<ScopedPtr<TimeseriesQuery>> queries;
  queries.emplace_back(new TimeseriesQuery());

  for (const auto param : params) {

    // param: metric
    if (param.first == "metric") {
      if (queries.back()->metric_key.length() > 0) {
        queries.emplace_back(new TimeseriesQuery());
      }

      queries.back()->metric_key = param.second;
      continue;
    }

    // param: aggr_fn
    if (param.first == "aggr_fn") {
      queries.back()->aggr_fn = TimeseriesQuery::aggrFnFromString(param.second);
      continue;
    }

    // param: aggr_window
    if (param.first == "aggr_window") {
      queries.back()->aggr_window =
          Duration(std::stod(param.second) * kMicrosPerSecond);
      continue;
    }

    // param: aggr_step
    if (param.first == "aggr_step") {
      queries.back()->aggr_step =
          Duration(std::stod(param.second) * kMicrosPerSecond);
      continue;
    }

    // param: group_by
    if (param.first == "group_by") {
      queries.back()->group_by.emplace_back(param.second);
      continue;
    }

    // param: join_with
    if (param.first == "join_with") {
      queries.back()->join_metric_key = param.second;
      continue;
    }

    // param: join_fn
    if (param.first == "join_fn") {
      queries.back()->join_fn = TimeseriesQuery::joinFnFromString(param.second);
      continue;
    }

    // param: join_aggr_fn
    if (param.first == "join_aggr_fn") {
      queries.back()->join_aggr_fn =
          TimeseriesQuery::aggrFnFromString(param.second);
      continue;
    }

    // param: scale
    if (param.first == "scale") {
      queries.back()->scale = std::stod(param.second);
      continue;
    }

    // param: draw
    if (param.first == "draw") {
      queries.back()->draw_style = param.second;
      continue;
    }

    // param: color
    if (param.first == "color") {
      queries.back()->color = param.second;
      continue;
    }

    // param: line_style
    if (param.first == "line_style") {
      queries.back()->line_style = param.second;
      continue;
    }

    // param: line_width
    if (param.first == "line_width") {
      queries.back()->line_width = param.second;
      continue;
    }

    // param: point_style
    if (param.first == "point_style") {
      queries.back()->point_style = param.second;
      continue;
    }

    // param: point_size
    if (param.first == "point_size") {
      queries.back()->point_size = param.second;
      continue;
    }

    // param: from
    if (param.first == "from") {
      //from = DateTime::fromTimespec(param.second);
      continue;
    }

    // param: until
    if (param.first == "until") {
      //until = DateTime::fromTimespec(param.second);
      continue;
    }

    /* param: format */
    if (param.first == "format") {
      resp_format = formatFromString(param.second);
      continue;
    }

    /* ignored params */
    if (param.first == "axis_left") continue;
    if (param.first == "axis_bottom") continue;
    if (param.first == "axis_right") continue;
    if (param.first == "axis_top") continue;
    if (param.first == "min") continue;
    if (param.first == "max") continue;
    if (param.first == "logarithmic") continue;
    if (param.first == "inverted") continue;
    if (param.first == "title") continue;
    if (param.first == "subtitle") continue;
    if (param.first == "width") continue;
    if (param.first == "height") continue;
    if (param.first == "grid") continue;
    if (param.first == "legend") continue;
    if (param.first == "legend_title") continue;

    RAISEF(kParseError, "invalid param: $0", param.first);
  }

  /* execute queries */
  for (const auto& query : queries) {
    query->run(from, until, metric_service_);
  }

  /* render results */
  switch (resp_format) {

    /* format: csv */
    case ResponseFormat::kCSV: {
      Buffer out;

      for (const auto& query : queries) {
        query->renderCSV(&out);
      }

      response->setStatus(http::kStatusOK);
      response->addBody(out);
      break;
    }

    /* format: svg */
    case ResponseFormat::kSVG: {
      chart::Canvas canvas;
      Vector<chart::Drawable*> charts;

      auto qbegin = queries.begin();
      auto qend = queries.end();

      while (qbegin != qend) {
        std::string style = (*qbegin)->draw_style;

        if (style == "lines") {
          renderCharts<chart::LineChart2D<DateTime, double>>(
              &canvas,
              style,
              &qbegin,
              qend,
              &charts);

          continue;
        }

        if (style == "area") {
          renderCharts<chart::AreaChart2D<DateTime, double>>(
              &canvas,
              style,
              &qbegin,
              qend,
              &charts);

          continue;
        }

        RAISEF(
            kIllegalArgumentError,
            "invalid draw style: $0",
            (*qbegin)->draw_style);
      }

      applyCanvasStyles(params, &canvas);
      applyChartStyles(params, from, until, &charts);

      Buffer out;
      BufferOutputStream outs(&out);
      chart::SVGTarget svgtarget(&outs);
      svgtarget.setViewbox(false);
      canvas.render(&svgtarget);

      response->setStatus(http::kStatusOK);
      response->addHeader("Content-Type", "text/html");
      response->addBody(out);
      break;
    }

    /* unknown format */
    default: {
      response->setStatus(http::kStatusBadRequest);
      response->addBody("unknown format");
      break;
    }

  }
}

void HTTPAPIServlet::valueQuery(
    http::HTTPRequest* request,
    http::HTTPResponse* response,
    URI* uri) {
  auto params = uri->queryParams();

  /* response format */
  auto resp_format = ResponseFormat::kCSV;
  std::string format_param;
  if (fnord::URI::getParam(params, "format", &format_param)) {
    resp_format = formatFromString(format_param);
  }

  /* metric key */
  std::string metric_key;
  if (!fnord::URI::getParam(params, "metric", &metric_key)) {
    RAISE(kIllegalArgumentError, "missing metric parameter");
  }

  auto smpl = metric_service_->getMostRecentSample(metric_key);

  /* render results */
  switch (resp_format) {

    /* format: csv */
    case ResponseFormat::kCSV: {
      Buffer out;

      out.append(
          StringUtil::format(
              "$0;$1;$2\n",
              metric_key,
              smpl.time(),
              smpl.value()));

      response->setStatus(http::kStatusOK);
      response->addBody(out);
      break;
    }

    /* unknown format */
    default: {
      response->setStatus(http::kStatusBadRequest);
      response->addBody("unknown format");
      break;
    }

  }
}

void HTTPAPIServlet::renderMetricJSON(
    fnord::metric_service::IMetric* metric,
    fnord::json::JSONOutputStream* json) const {
  json->beginObject();

  json->addObjectEntry("key");
  json->addString(metric->key());
  json->addComma();

  json->addObjectEntry("total_bytes");
  json->addInteger(metric->totalBytes());
  json->addComma();

  json->addObjectEntry("last_insert");
  json->addInteger(static_cast<uint64_t>(metric->lastInsertTime()));
  json->addComma();

  json->addObjectEntry("labels");
  json->beginArray();
  auto labels = metric->labels();
  for (auto cur = labels.begin(); cur != labels.end(); ++cur) {
    if (cur != labels.begin()) {
      json->addComma();
    }
    json->addString(*cur);
  }
  json->endArray();

  json->endObject();
}

HTTPAPIServlet::ResponseFormat HTTPAPIServlet::formatFromString(
    const String& format) {
  if (format == "csv") {
    return ResponseFormat::kCSV;
  }

  if (format == "json") {
    return ResponseFormat::kJSON;
  }

  if (format == "svg") {
    return ResponseFormat::kSVG;
  }

  RAISEF(kIllegalArgumentError, "invalid format: $0", format);
}

void HTTPAPIServlet::applyChartStyles(
    const URI::ParamList& params,
    const DateTime& from,
    const DateTime& until,
    Vector<chart::Drawable*>* charts) {
  if (charts->size() == 0) {
    return;
  }

  bool axis_left = true;
  bool axis_bottom = true;
  bool axis_right = false;
  bool axis_top = false;

  for (const auto param : params) {

    // param: axis_left
    if (param.first == "axis_left") {
      axis_left = (param.second == "true");
      continue;
    }

    // param: axis_bottom
    if (param.first == "axis_bottom") {
      axis_bottom = (param.second == "true");
      continue;
    }

    // param: axis_right
    if (param.first == "axis_right") {
      axis_right = (param.second == "true");
      continue;
    }

    // param: axis_top
    if (param.first == "axis_top") {
      axis_top = (param.second == "true");
      continue;
    }

    // param: max
    if (param.first == "max") {
      for (const auto& chart : *charts) {
        auto ydomain = dynamic_cast<chart::ContinuousDomain<double>*>(
            chart->getDomain(chart::AnyDomain::DIM_Y));

        if (ydomain != nullptr) {
          ydomain->setMax(std::stod(param.second));
        }
      }

      continue;
    }

    // param: min
    if (param.first == "min") {
      for (const auto& chart : *charts) {
        auto ydomain = dynamic_cast<chart::ContinuousDomain<double>*>(
            chart->getDomain(chart::AnyDomain::DIM_Y));

        if (ydomain != nullptr) {
          ydomain->setMin(std::stod(param.second));
        }
      }

      continue;
    }

    // param: logarithmic
    if (param.first == "logarithmic" && param.second == "true") {
      for (const auto& chart : *charts) {
        auto ydomain = dynamic_cast<chart::ContinuousDomain<double>*>(
            chart->getDomain(chart::AnyDomain::DIM_Y));

        if (ydomain != nullptr) {
          ydomain->setLogarithmic(true);
        }
      }

      continue;
    }

    // param: inverted
    if (param.first == "inverted" && param.second == "true") {
      for (const auto& chart : *charts) {
        auto ydomain = dynamic_cast<chart::ContinuousDomain<double>*>(
            chart->getDomain(chart::AnyDomain::DIM_Y));

        if (ydomain != nullptr) {
          ydomain->setInverted(true);
        }
      }

      continue;
    }

    // param: grid
    if (param.first == "grid") {
      if (param.second == "horizontal") {
        charts->front()->addGrid(chart::GridDefinition::GRID_HORIZONTAL);
        continue;
      }

      if (param.second == "vertical") {
        charts->front()->addGrid(chart::GridDefinition::GRID_VERTICAL);
        continue;
      }

      if (param.second == "both") {
        charts->front()->addGrid(chart::GridDefinition::GRID_HORIZONTAL);
        charts->front()->addGrid(chart::GridDefinition::GRID_VERTICAL);
        continue;
      }

      RAISEF(kIllegalArgumentError, "invalid value for grid: $0", param.second);
    }

    // param: legend
    if (param.first == "legend") {
      std::string legend_title;
      URI::getParam(params, "legend_title", &legend_title);

      if (param.second == "topleftinside") {
        charts->front()->addLegend(
            chart::LegendDefinition::LEGEND_TOP,
            chart::LegendDefinition::LEGEND_LEFT,
            chart::LegendDefinition::LEGEND_INSIDE,
            legend_title);

        continue;
      }

      if (param.second == "topleftoutside") {
        charts->front()->addLegend(
            chart::LegendDefinition::LEGEND_TOP,
            chart::LegendDefinition::LEGEND_LEFT,
            chart::LegendDefinition::LEGEND_OUTSIDE,
            legend_title);

        continue;
      }

      if (param.second == "toprightinside") {
        charts->front()->addLegend(
            chart::LegendDefinition::LEGEND_TOP,
            chart::LegendDefinition::LEGEND_RIGHT,
            chart::LegendDefinition::LEGEND_INSIDE,
            legend_title);

        continue;
      }

      if (param.second == "toprightoutside") {
        charts->front()->addLegend(
            chart::LegendDefinition::LEGEND_TOP,
            chart::LegendDefinition::LEGEND_RIGHT,
            chart::LegendDefinition::LEGEND_OUTSIDE,
            legend_title);

        continue;
      }

      if (param.second == "bottomleftinside") {
        charts->front()->addLegend(
            chart::LegendDefinition::LEGEND_BOTTOM,
            chart::LegendDefinition::LEGEND_LEFT,
            chart::LegendDefinition::LEGEND_INSIDE,
            legend_title);

        continue;
      }

      if (param.second == "bottomleftoutside") {
        charts->front()->addLegend(
            chart::LegendDefinition::LEGEND_BOTTOM,
            chart::LegendDefinition::LEGEND_LEFT,
            chart::LegendDefinition::LEGEND_OUTSIDE,
            legend_title);

        continue;
      }

      if (param.second == "bottomrightinside") {
        charts->front()->addLegend(
            chart::LegendDefinition::LEGEND_BOTTOM,
            chart::LegendDefinition::LEGEND_RIGHT,
            chart::LegendDefinition::LEGEND_INSIDE,
            legend_title);

        continue;
      }

      if (param.second == "bottomrightoutside") {
        charts->front()->addLegend(
            chart::LegendDefinition::LEGEND_BOTTOM,
            chart::LegendDefinition::LEGEND_RIGHT,
            chart::LegendDefinition::LEGEND_OUTSIDE,
            legend_title);

        continue;
      }

      RAISEF(
          kIllegalArgumentError,
          "invalid value for legend: $0",
          param.second);
    }

  }

  for (const auto& chart : *charts) {
    auto xdomain = dynamic_cast<chart::ContinuousDomain<DateTime>*>(
        chart->getDomain(chart::AnyDomain::DIM_X));

    if (xdomain != nullptr) {
      xdomain->setMin(from);
      xdomain->setMax(until);
    }
  }

  if (axis_left) {
    charts->front()->addAxis(chart::AxisDefinition::LEFT);
  }

  if (axis_bottom) {
    charts->front()->addAxis(chart::AxisDefinition::BOTTOM);
  }

  if (axis_right) {
    charts->front()->addAxis(chart::AxisDefinition::RIGHT);
  }

  if (axis_top) {
    charts->front()->addAxis(chart::AxisDefinition::TOP);
  }
}

void HTTPAPIServlet::applyCanvasStyles(
    const URI::ParamList& params,
    chart::Canvas* canvas) {
  int width = 800;
  int height = 320;

  for (const auto param : params) {

    // param: title
    if (param.first == "title") {
      canvas->setTitle(param.second);
      continue;
    }

    // param: subtitle
    if (param.first == "subtitle") {
      canvas->setSubtitle(param.second);
      continue;
    }

    // param: width
    if (param.first == "width") {
      width = std::stoi(param.second);
      continue;
    }

    // param: height
    if (param.first == "height") {
      height = std::stoi(param.second);
      continue;
    }
  }

  canvas->setDimensions(width, height);
}

}
}
