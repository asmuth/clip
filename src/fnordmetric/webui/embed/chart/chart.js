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
    var query_manager = new FnordMetricChart.QueryManager(params);
    query_manager.executeQueries(callback, function() {
      // handle error
    });
  }

  function renderSkeletonHTML() {
    var container_elem = document.createElement("div");
    container_elem.classList.add("fm-chart-container");

    var title_elem = document.createElement("div");
    title_elem.classList.add("fm-chart-title");
    container_elem.appendChild(title_elem);

    var body_elem = document.createElement("div");
    body_elem.classList.add("fm-chart-body");
    container_elem.appendChild(body_elem);

    var summary_elem = document.createElement("div");
    summary_elem.classList.add("fm-chart-summary");
    body_elem.appendChild(summary_elem);

    var plot_elem = document.createElement("div");
    plot_elem.classList.add("fm-chart-plot");
    body_elem.appendChild(plot_elem);

    var series_list_elem = document.createElement("div");
    series_list_elem.classList.add("fm-chart-series-list");
    container_elem.appendChild(series_list_elem);

    viewport.appendChild(container_elem);
  }

  function renderChart(result) {
    var elem = viewport.querySelector(".fm-chart-plot");
    var plotter = new FnordMetricChart.Plotter(elem, params);
    plotter.render(result);
  }

  function renderSummaries(result) {

  }

  function renderSeriesList(result) {

  }

};

