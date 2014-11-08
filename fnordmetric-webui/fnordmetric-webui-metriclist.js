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

if (FnordMetric.views === undefined) {
  FnordMetric.views = {};
}

FnordMetric.views.MetricList = function() {
  function render(viewport, url) {
    loadMetricList(viewport);
  };

  function renderMetricList(viewport, metrics) {
    if (metrics.length == 0) {
      renderEmptyMetricsList(viewport);
    } else {
      viewport.innerHTML = "FIXME: render metrics table";
      console.log(metrics);
    }
  };

  function renderEmptyMetricsList(elem) {
    var msg_field = document.createElement("div");
    msg_field.className = "metrics_error_pane";
    msg_field.innerHTML = "Looks like you haven't inserted any data yet.";
    elem.innerHTML = "";
    elem.appendChild(msg_field);
  }

  function loadMetricList(viewport) {
    FnordMetric.httpGet("/metrics", function(r) {
      if (r.status == 200) {
        var json = JSON.parse(r.response);
        renderMetricList(viewport, json.metrics);
      } else {
        FnordMetric.util.displayErrorMessage(viewport, "Error connecting to server");
      }
    });
  }

  var destroy = function(elem) {
  }

  return {
    "render": render,
    "destroy": destroy
  };
};
