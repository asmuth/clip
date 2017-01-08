/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

function FnordMetricMetricList(elem, params) {
  'use strict';

  var api_url = "/list";

  this.initialize = function() {
    initLayout();

    fetchData(function(result) {
      renderTable(result);
    });
  };

  //this.destroy = function() {
  //};

  function initLayout() {
    var page = templateUtil.getTemplate("fnordmetric-metric-list-tpl");
    elem.appendChild(page);
  }

  var fetchData = function(callback) {
    var url = params.app.api_base_path + api_url;

    HTTPUtil.httpGet(url, {}, function(r) {
      if (r.status != 200) {
        params.app.renderError(
            "an error occured while loading the metric list:",
            r.response);
        return;
      }

      var metrics = JSON.parse(r.response);
      callback(metrics);
    });
  };

  var renderTable = function(result) {
    var table = new FnordMetricMetricList.Table(
        elem.querySelector(".fnordmetric-metric-list table.metric_list"));
    table.render(result);

    table.onClick(function(metric_id) {
      params.app.navigateTo(params.route.route + "/" + metric_id);
    });
  }
};

FnordMetric.views["fnordmetric.metric.list"] = FnordMetricMetricList;
