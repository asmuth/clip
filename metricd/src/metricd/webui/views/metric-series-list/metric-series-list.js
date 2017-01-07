/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

FnordMetric.views["fnordmetric.metric.series.list"] = function(elem, params) {
  'use strict';

  var metric_id = params.route.args.metric_id;

  this.initialize = function() {
    renderLayout();
    renderSummary();
  };

  this.destroy = function() {

  };

  function renderLayout() {
    var page = templateUtil.getTemplate("fnordmetric-metric-series-list-tpl");
    elem.appendChild(page);
  }

  function renderSummary() {
    var chart_opts = {
      metric_id: metric_id,
      axis_y_position: "inside",
      border_top: false,
      border_right: false,
      border_bottom: true,
      border_left: false
    };

    console.log(elem.querySelector(".summary"));
    var chart = new FnordMetricChart(elem.querySelector(".summary"), chart_opts);
    chart.render();
  }

  function renderSeriesList() {

  }

  //var getParams = function(path) {
  //  var p = {};

  //  p.metric = params.route.args[1];

  //  var start_param = URLUtil.getParamValue(path, "start");
  //  if (start_param) {
  //    p.start = parseInt(start_param);
  //  }

  //  var end_param = URLUtil.getParamValue(path, "end");
  //  if (end_param) {
  //    p.end = parseInt(end_param);
  //  }

  //  var filter_param = URLUtil.getParamValue(path, "filter");
  //  if (filter_param) {
  //    p.filter = filter_param;
  //  }

  //  var order_col_param = URLUtil.getParamValue(path, "order_col");
  //  if (order_col_param) {
  //    p.order_col = order_col_param;
  //  }

  //  var order_dir_param = URLUtil.getParamValue(path, "order_dir");
  //  if (order_dir_param) {
  //    p.order_dir = order_dir_param;
  //  }

  //  return p;
  //}

  //var updateQueryStr = function(query_params) {
  //  var url = params.path;
  //  for (var k in query_params) {
  //    url = URLUtil.addOrModifyParam(url, k, query_params[k]);
  //  }

  //  params.app.navigateTo(url);
  //}

  //var watchTimeRangePicker = function() {
  //  var picker = elem.querySelector(
  //      ".fnordmetric-metric-series-list f-timerange-picker");

  //  var timerange = {};

  //  var timezone = DomUtil.getCookie("timezone");
  //  if (timezone) {
  //    timerange.timezone = timezone;
  //  }

  //  if (url_params.start && url_params.end) {
  //    timerange.start = url_params.start;
  //    timerange.end = url_params.end;
  //  }

  //  picker.initialize(timerange);

  //  picker.addEventListener("submit", function(e) {
  //    updateTimezoneCookie(this.getTimezone());
  //    updateQueryStr(this.getTimerange());
  //  }, false);
  //}

};

