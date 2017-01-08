/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

FnordMetricMetricList.Table = function(elem) {
  'use strict';

  this.render = function(result) {
    renderHeader();
    renderBody(result);
  }

/********************************** private ***********************************/

  function renderHeader() {
    var head_tr = elem.querySelector("thead");
    var columns = [
      {
        key: "metric_id",
        title: "Metric ID",
        sortable: true
      }
    ];

    columns.forEach(function(col) {
      head_tr.appendChild(renderHeaderCell(col));
    });
  }

  function renderHeaderCell(col) {
    var th = document.createElement("th");
    th.classList.add(col.key);

    if (col.title) {
      th.innerHTML = DOMUtil.escapeHTML(col.title);
    }

    if (col.sortable) {
      var sort_elem = document.createElement("span");
      sort_elem.classList.add("sort");
      th.appendChild(sort_elem);

      var sort_desc = document.createElement("i");
      sort_desc.classList.add("sort_desc");
      sort_elem.appendChild(sort_desc);

      var sort_asc = document.createElement("i");
      sort_asc.classList.add("sort_asc");
      sort_elem.appendChild(sort_asc);

      switch (col.sorted) {
        case "asc":
          sort_asc.classList.add("active");
          break;
        case "desc":
          sort_desc.classList.add("active");
          break;
      }

      var sort_fn = function(dir) {
        on_sort.forEach(function(f) {
          f(c, dir);
        });
      };

      DOMUtil.onClick(sort_asc, function() {
        sort_fn("asc");
      });

      DOMUtil.onClick(sort_desc, function() {
        sort_fn("desc");
      });
    }

    if (col.attrs) {
      for (var attr in col.attrs) {
        th.setAttribute(attr, col.attrs[attr]) //FIXME escape!
      }
    }

    return th;
  }

  function renderBody(result) {
    var tbody = elem.querySelector("tbody");

    result.metrics.forEach(function(metric) {
      tbody.appendChild(renderRow(metric));
    });
  }

  function renderRow(metric) {
    var tr = document.createElement("tr");

    var td = document.createElement("td");
    td.innerHTML = DOMUtil.escapeHTML(metric.metric_id);
    tr.appendChild(td);

    return tr;
  }

}

