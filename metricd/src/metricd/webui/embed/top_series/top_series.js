/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
var FnordMetricTopSeries = function(viewport, params) {
  'use strict';

  this.render = function() {
    renderSkeletonHTML();

    fetchData(function(result) {
      renderTable(result);
    });
  };

  function fetchData(callback) {
    //FIXME
    //REMOVE ME
    HTTPUtil.httpPost(
        "/api/v1/metrics/fetch_series",
        JSON.stringify({metric_id: params.metric_id}),
        {},
        function(r) {
      if (r.status != 200) {
        success = false;
        return;
      }

      var result = {
        series: []
      }

      var r = JSON.parse(r.response);
      r.series.forEach(function(s) {
        result.series.push(s);
      });
      callback(result);

    });
    //REMOVE ME END
  }

  function renderSkeletonHTML() {
    var container_elem = document.createElement("div");
    container_elem.classList.add("fm-top-series-container");

    var table_elem = document.createElement("table");
    table_elem.classList.add("fm-top-series-table");
    container_elem.appendChild(table_elem);

    var thead_elem = document.createElement("thead");
    table_elem.appendChild(thead_elem);

    var tbody_elem = document.createElement("tbody");
    table_elem.appendChild(tbody_elem);

    viewport.appendChild(container_elem);
  }

  function renderTable(result) {
    var elem = viewport.querySelector(".fm-top-series-table");
    var table = new FnordMetricTopSeries.Table(elem);
    table.render(result);

    var table_event_handler = new FnordMetricTopSeries.TableEventHandler(table);
    table_event_handler.init();
  }
}

