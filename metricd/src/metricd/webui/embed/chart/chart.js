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
    var url = "/api/v1/metrics/fetch?metric_id=prod.db.mysql.ram_used";

    HTTPUtil.httpGet(url, {}, function(r) {
      if (r.status != 200) {
        // FIXME
        return;
      }

      var result = JSON.parse(r.response);
      console.log(result);
      callback(result);
    });
  }

  function renderSkeletonHTML() {
    // FIXME
  }

  function renderChart(result) {
    var elem = viewport.querySelector("fm-chart-plot");
    var plotter = new FnordMetricChart.Plotter(elem, params);
    plotter.render();
  }

  function renderSummaries(result) {

  }

  function renderSeriesList(result) {

  }

};

