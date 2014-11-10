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
  var actions = {
    "search" : FnordMetric.util.searchMetricList,
    "metric" : FnordMetric.util.getSingleMetric
  }

  function render(viewport, url, query_params) {
    loadMetricList(viewport, query_params);
  };


  function onRowClick(e) {
    e.preventDefault();
    var metric = this.firstChild.id;
    //FIXME
    FnordMetric.util.setURLQueryString(
      "metric", metric, false);
    location.reload();
  };

  function renderMetricButton(elem, metric) {
    var button = FnordMetric.createButton(
      "#", undefined, "Open in Query Editor");
    elem.appendChild(button);

    button.onclick = function(e) {
      e.preventDefault();
      var query = 
        "SELECT * FROM " + metric[0].key;
      FnordMetric.util.setFragmentURL(
        "query_playground", "sql_query", query, true);
      location.reload();
    }
  }

  function renderMetricList(viewport, metrics, action) {
    viewport.innerHTML = "";
    var table_container = document.createElement("div");
    viewport.appendChild(table_container);

    if (metrics.length == 0) {
      if (action != undefined) {
        console.log("render no search result");
      } else {
        renderEmptyMetricsList(viewport);
      }
      return;
    }

    if (action == "metric") {
      renderMetricButton(viewport, metrics);
    }

    var table_view = FnordMetric.util.TableView([
        "Metric",
        "Labels",
        "Last Insert",
        "Total stored bytes"], table_container, 25);

    for (i in metrics) {
      table_view.addRow([
          metrics[i]["key"],
          FnordMetric.util.convertArrayToString(
              metrics[i]["labels"]),
          FnordMetric.util.parseTimestamp(
              metrics[i]["last_insert"]),
          metrics[i]["total_bytes"]]);
    }

    table_view.onRowClick(onRowClick);
    table_view.render();
  };

  function renderEmptyMetricsList(elem) {
    var msg_field = document.createElement("div");
    msg_field.className = "metrics_error_pane";
    msg_field.innerHTML = "Looks like you haven't inserted any data yet.";
    elem.innerHTML = "";
    elem.appendChild(msg_field);
  }

  function loadMetricList(viewport, query_params) {
    FnordMetric.httpGet("/metrics", function(r) {
      if (r.status == 200) {
        var json = JSON.parse(r.response);
        var action;
        if (query_params != undefined) {
          json.metrics =
              actions[query_params.name](json.metrics, query_params.value);
          action = query_params.name;
        }
        renderMetricList(viewport, json.metrics, action);
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
