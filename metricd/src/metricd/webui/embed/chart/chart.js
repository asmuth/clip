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

    fetchData(function(series) {
      renderChart(series);
      renderSummaries(series);
      renderSeriesList(series);
    });
  };

  function fetchData(callback) {
    var query_manager = new FnordMetricChart.QueryManager(params);
    query_manager.executeQueries(callback, function() {
      // handle error
    });
  }

  function renderSkeletonHTML() {
    viewport.innerHTML = '<div class="fm-chart-container"> <div class="fm-chart-title"> </div> <div class="fm-chart-body"> <div class="fm-chart-summary"> </div> <div class="fm-chart-plot"> </div> </div> <div class="fm-chart-series-list"> </div> </div>';
  }

  function renderChart(series) {
    var elem = viewport.querySelector(".fm-chart-plot");
    var plotter = new FnordMetricChart.Plotter(elem, params);
    plotter.render(series);
  }

  function renderSummaries(series) {

  }

  function renderSeriesList(series) {

  }

};

