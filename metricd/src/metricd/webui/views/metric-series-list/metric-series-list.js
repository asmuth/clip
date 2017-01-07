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

  this.initialize = function() {
    initLayout();
    refreshSummary();
  };

  this.destroy = function() {

  };

  function refresh(what_changed) {
    refreshSummary();
    refreshSeriesList();
  }

  function refreshSummary() {
    var chart_opts = {
      axis_y_position: "inside",
      border_top: false,
      border_right: false,
      border_bottom: true,
      border_left: false,
      queries: [
        {
          query: {
            op: "fetch_summary",
            metric_id: config.getMetricID()
          }
        }
      ]
    };

    var chart = new FnordMetricChart(elem.querySelector(".summary"), chart_opts);
    chart.render();
  }

  function refreshSeriesList() {

  }

  function initLayout() {
    var page = templateUtil.getTemplate("fnordmetric-metric-series-list-tpl");
    elem.appendChild(page);

    /** initialize ui components **/
    initHeaderBreadcrumbs();
    initLayoutDatepicker();
    initLayoutFilter();
  }

  function initHeaderBreadcrumbs() {
    elem.querySelector(".page_header .breadcrumbs .metric_name").innerHTML =
      DOMUtil.escapeHTML(config.getMetricID());
  }

  function initLayoutDatepicker() {
    var picker = new FM.TimeRangePickerComponent(
        elem.querySelector("f-timerange-picker"));

    picker.initialize({
      timezone: config.getTimezone(),
      from: config.getFrom(),
      until: config.getUntil()
    });

    picker.onSubmit(function(timerange) {
      refresh(timerange);
    });
  }

  function initLayoutFilter() {
    var filter = new FM.FilterComponent(elem.querySelector(".search input"));
    filter.init(config.getFilter());

    filter.onSubmit(function(filter_value) {
      refresh({filter: filter_value});
    });
  }

}

FnordMetric.views["fnordmetric.metric.series.list"] = FnordMetricSeriesListView;
