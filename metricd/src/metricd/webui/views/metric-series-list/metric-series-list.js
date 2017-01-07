/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

function FnordMetricSeriesListView(elem, params) {
  'use strict';

  var config = new FnordMetricSeriesListViewConfig(params);
  var metric_id = params.route.args.metric_id;

  this.initialize = function() {
    initLayout();
    refreshSummary();
  };

  this.destroy = function() {

  };

  function refresh(var what_changed) {
    refreshSummary();
    refreshSeriesList();
  }

  function refreshSummary() {
    var chart_opts = {
      metric_id: config.getMetricID(),
      axis_y_position: "inside",
      border_top: false,
      border_right: false,
      border_bottom: true,
      border_left: false
    };

    var chart = new FnordMetricChart(elem.querySelector(".summary"), chart_opts);
    chart.render();
  }

  function refreshSeriesList() {

  }

  function initLayout() {
    var page = templateUtil.getTemplate("fnordmetric-metric-series-list-tpl");
    elem.appendChild(page);

    // initialize ui components
    initLayoutDatepicker();
    initLayoutDatepicker();
    initLayoutDatepicker();
    initLayoutDatepicker();
  }

  function initLayoutDatepicker() {

  }

}

FnordMetric.views["fnordmetric.metric.series.list"] = FnordMetricSeriesListView;
