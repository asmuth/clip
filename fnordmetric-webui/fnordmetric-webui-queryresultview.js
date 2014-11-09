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
    elem.innerHTML = "";
    var chart_pane = document.createElement("div");
    chart_pane.className = "chart_container";
    chart_pane.setAttribute("id", "chart_container");
    if (chart != undefined) {
      chart_pane.innerHTML = chart.svg;
    }
    elem.appendChild(chart_pane);
  }

  function renderChartPane(charts, elem) {
    if (charts == undefined) {
      return;
    }
    var navbar = document.createElement("div");
    navbar.className = "result_navbar";
    for (var i = 0; i < charts.length; i++) {
      var item = FnordMetric.createButton(
        "#",
        "result_link",
        "<h3>Chart "+(i+1)+"</h3>");
      item.setAttribute("id", i);
      navbar.appendChild(item);

      item.onclick = function(e) {
        e.preventDfault();
        renderChart(charts[this.id], chart_container);
      }
    }
    elem.appendChild(navbar);
    var chart_container = document.createElement("div");
    elem.appendChild(chart_container);
    renderChart(charts[0], chart_container);
  }

  function renderTable(table, elem) {
    var table_view = FnordMetric.util.TableView(
      table.columns, elem);

    table.rows.map(function(row) {
      table_view.addRow(row);
    });
    table_view.render(false);
  }


  function renderTablePane(tables, elem) {
    if (tables == undefined) {
      return;
    }
    var navbar = document.createElement("div");
    navbar.className = "result_navbar";
    for (var i = 0; i < tables.length; i++) {
      var item = FnordMetric.createButton(
        "#",
        "result_link", 
        "<h3>Table "+(i+1)+"</h3>");
      item.setAttribute("id", i);
      navbar.appendChild(item);

      item.onclick = function(e) {
        e.preventDefault();
        renderTable(tables[this.id], table_container);
      };
    }
    elem.appendChild(navbar);
    var table_container = document.createElement("div");
    elem.appendChild(table_container);
    renderTable(tables[0], table_container);
    result_pane.style.height = "auto";
  }

  function renderError(msg, elem) {
    var msg_field = document.createElement("div");
    msg_field.className = "error_field";
    msg_field.innerHTML = msg;
    elem.appendChild(msg_field);
  }


  function render(elem, resp, duration) {
    elem.innerHTML = "";
    console.log("render");
    console.log(resp);
    if (resp.status == "success") {
      var charts = resp.charts;
      renderChartPane(charts, elem);
      var tables = resp.tables;
      renderTablePane(tables, elem);
    } else {
      renderError(resp.error, elem);
    }
  }

  return {
    "render" : render
  }
}
