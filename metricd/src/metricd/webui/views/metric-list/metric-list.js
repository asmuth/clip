/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

FnordMetric.views["fnordmetric.metric.list"] = function(elem, params) {
  'use strict';

  var api_url = "/list";
  var table = new fTable({
    columns: [
      {
        key: "metric_id",
        title: "Metric ID"
      }
    ]
  });

  this.initialize = function() {
    var page = templateUtil.getTemplate("fnordmetric-metric-list-tpl");
    elem.appendChild(page);

    /* navigate to id detail page */
    table.onClick(function(r) {
      params.app.navigateTo(params.route.route + "/" + r.cells.metric_id.value);
    });

    /* sort callback */
    table.onSort(function(column, direction) {
      params.view_cfg.updateValue("order", direction);
      params.view_cfg.updateValue("order_by", column.key);
      updatePath();
    });

    table.render(
        elem.querySelector(".fnordmetric-metric-list table.metric_list"));

    fetchMetricList();
  };

  this.destroy = function() {
  };

  var fetchMetricList = function() {
    HTTPUtil.httpGet(params.app.api_base_path + api_url, {}, function(r) {
      if (r.status != 200) {
        params.app.renderError(
            "an error occured while loading the metric list:",
            r.response);
        return;
      }

      var metrics = JSON.parse(r.response);
      renderTable(metrics.metrics);
    });
  };

  var renderTable = function(metrics) {
    var rows = [];
    metrics.forEach(function(m) {
      var cells = {};
      for (var k in m) {
        cells[k] = {
          value: m[k]
        }
      }

      rows.push({cells});
    });

    table.setRows(rows);
    table.render(
        elem.querySelector(".fnordmetric-metric-list table.metric_list"));
  };
};
