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

if (FnordMetric.util === undefined) {
  FnordMetric.util = {};
}

FnordMetric.util.queryResultView = function() {

  function renderChart(chart, elem) {
    var chart_container = document.createElement("div");
    chart_container.className = "chart_container";
    chart_container.setAttribute("id", "chart_container");
    if (chart != undefined) {
      chart_container.innerHTML = chart.svg;
    }
    elem.appendChild(chart_container);
  }

  function renderChartNavbar() {
    var navbar = document.createElement("div");
    navbar.className = "result_navbar";
  }






  function render(elem, resp, duration) {
    console.log("render Result");
    console.log(resp);
    elem.innerHTML = "";



  }

  return {
    "render" : render
  }
}
