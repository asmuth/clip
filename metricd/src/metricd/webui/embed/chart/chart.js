/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
var FnordMetricChart = function(viewport, params) {

  this.render = function() {
    renderSkeletonHTML();

    fetchData(function(result) {
      renderChart(result);
      renderSummaries(result);
      renderSeriesList(result);
    });
  };

  function fetchData(callback) {
    var url = "/api/v1/metrics/fetch?metric_id=" + params.metric_id;

    HTTPUtil.httpGet(url, {}, function(r) {
      if (r.status != 200) {
        // FIXME
        return;
      }

      var result = JSON.parse(r.response);
      callback(result);
    });
  }

  function renderSkeletonHTML() {
    viewport.innerHTML = '<div class="fm-chart-container"> <div class="fm-chart-title"> </div> <div class="fm-chart-body"> <div class="fm-chart-summary"> </div> <div class="fm-chart-plot"> </div> </div> <div class="fm-chart-series-list"> </div> </div>';
  }

  function renderChart(result) {
    var elem = viewport.querySelector(".fm-chart-plot");
    var plotter = new FnordMetricChart.Plotter(elem, params);
    plotter.render(result.series);
  }

  function renderSummaries(result) {

  }

  function renderSeriesList(result) {

  }

};

