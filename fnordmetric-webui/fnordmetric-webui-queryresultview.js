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

FnordMetric.util.queryResultView = function(query_str) {

  function renderChart(chart, elem) {
    elem.innerHTML = "";
    var chart_pane = document.createElement("div");
    chart_pane.className = "chart_container";
    chart_pane.setAttribute("id", "chart_container");

    var controls = document.createElement("div");
    controls.className = "metric_preview_secondary_controls";
    controls.innerHTML = "<a style='float: left;'>Result Chart</a>"
    chart_pane.appendChild(controls);

    var controls_embed = FnordMetric.createButton(
      "#", "btn", "<i class='fa fa-share'></i> Embed");
    controls.appendChild(controls_embed);
    controls_embed.onclick = function(e) {
      e.preventDefault();
      FnordMetric.util.embedPopup(elem, query_str).render()
    }

    var controls = document.createElement("div");

    if (chart != undefined) {
      var svg = document.createElement("div");
      svg.className = "svg";
      svg.innerHTML = chart.svg;
      chart_pane.appendChild(svg);
    }
    elem.appendChild(chart_pane);
  }

  function renderChartPane(charts, elem) {
    if (charts == undefined) {
      return;
    }

    var chart_container = document.createElement("div");
    chart_container.className = "result_card";
    elem.appendChild(chart_container);
    renderChart(charts[0], chart_container);
  }

  function renderTable(table, elem) {
    var controls = document.createElement("div");
    controls.className = "metric_preview_secondary_controls";
    controls.innerHTML = "<a style='float: left;'>Result Table</a>"
    elem.appendChild(controls);


    var table_elem = document.createElement("div");
    table_elem.className = "result_table";
    elem.appendChild(table_elem);

    var table_view = FnordMetric.util.TableView(
      table.columns, table_elem, 20);

    table.rows.map(function(row) {
      table_view.addRow(row);
    });

    table_view.render(false);
  }


  function renderTablePane(tables, elem) {
    if (tables == undefined) {
      return;
    }

    for (var i = 0; i < tables.length; i++) {
      var table_container = document.createElement("div");
      table_container.className = "result_card";
      elem.appendChild(table_container);
      renderTable(tables[i], table_container);
    }
  }

  function renderError(msg, elem) {
    var msg_field = document.createElement("div");
    msg_field.className = "error_box";
    msg_field.innerHTML = msg;
    elem.appendChild(msg_field);
  }


  function render(elem, resp, duration) {
    elem.innerHTML = "";
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
