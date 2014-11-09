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

  function renderTable(table, elem) {
    var table_view = FnordMetric.util.TableView(
      table.columns, elem);

    table.rows.map(function(row) {
      table_view.addRow(row);
    });

    table_view.render();
  }


  function renderTableNavbar(tables, elem) {
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
  }



  function render(elem, resp, duration) {
    elem.innerHTML = "";
    var tables = resp.tables;
    renderTableNavbar(tables, elem);



  }

  return {
    "render" : render
  }
}
