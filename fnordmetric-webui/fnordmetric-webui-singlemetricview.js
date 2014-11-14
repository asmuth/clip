/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Laura Schlimmer
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
if (FnordMetric === undefined) {
  FnordMetric = {};
}

if (FnordMetric.views === undefined) {
  FnordMetric.views = {};
}


FnordMetric.util.singleMetricView = function() {
  function render(elem, query_params) {
    /* set url and push history state */

    /*FnordMetric.util.setURLQueryString(
      "metric_list", query_params, false, true);*/
    elem.innerHTML = "";


    FnordMetric.util.renderPageHeader(
      "Metric &rsaquo; <em>" + 
      FnordMetric.util.htmlEscape(query_params.innerViewValue) + 
      "</em>", elem);

    var preview_widget = FnordMetric.util.MetricPreviewWidget(
      elem, query_params);
    preview_widget.render();

  }

  return {
    "render" : render
  }
}
