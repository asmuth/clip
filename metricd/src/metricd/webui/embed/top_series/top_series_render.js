/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

FnordMetricTopSeries.Renderer = function(elem, params) {
  'use strict';

  var columns = [
    {
      key: "series_id",
      title: "Series ID",
      sortable: true
    },
    {
      key: "summary",
      title: "Value",
      sortable: true,
      attrs: {
        colspan: 2
      }
    },
    {
      key: "menu",
      sortable: false
    }
  ];

  this.render = function(result) {
    renderHeader();
  }

  function renderHeader() {
    var head_tr = elem.querySelector("thead");

    columns.forEach(function(col) {
      var th = document.createElement("th");
      th.classList.add(col.key);

      if (col.title) {
        th.innerHTML = DOMUtil.escapeHTML(col.title);
      }

      if (col.sortable) {
        var sort_elem = document.createElement("span");
        sort_elem.classList.add("sort");
        th.appendChild(sort_elem);

        var sort_asc = document.createElement("i");
        sort_asc.classList.add("sort_asc");
        sort_elem.appendChild(sort_asc);

        var sort_desc = document.createElement("i");
        sort_desc.classList.add("sort_desc");
        sort_elem.appendChild(sort_desc);

        switch (col.sorted) {
          case "asc":
            sort_asc.classList.add("active");
            break;
          case "desc":
            sort_desc.classList.add("active");
            break;
        }

        var sort_fn = function(dir) {
          //on_sort.forEach(function(f) {
          //  f(c, dir);
          //});
        };

        DOMUtil.onClick(sort_asc, function() {
          sort_fn("asc");
        });

        DOMUtil.onClick(sort_desc, function() {
          sort_fn("desc");
        });
      }

      head_tr.appendChild(th);
    });
  }

  function renderRow() {
    renderIDCell();
    renderSparklineCell();
    renderSummaryCell();
    renderContextMenuCell();
  }

  function renderIDCell() {

  }

  function renderSparklineCell(series) {
    var plotter = new FnordMetricTopSeries.SparklinePlotter(some_elem);
    plotter.render(series);
  }

  function renderSummaryCell() {

  }

  function renderContextMenuCell() {

  }
}

