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
    "search" : {
      "render" : loadMetricList,
      "data" :FnordMetric.util.searchMetricList
    },
    "metric" : {
      "render" : FnordMetric.util.singleMetricView().render
    }
  }


  function render(viewport, url, query_params) {
    if (query_params != undefined) {
      actions[query_params.name].render(viewport, query_params);
      return;
    }
    loadMetricList(viewport, query_params);
  };


  function onRowClick() {
    //FIXME
    var viewport = document.body.querySelector(".viewport");
    var params = {
      "name" : "metric",
      "value" : this.firstChild.id
    };
    FnordMetric.util.singleMetricView().render(
      viewport, params);
  };



  function renderMetricList(viewport, metrics, search_item) {
    viewport.innerHTML = "";

    if (metrics.length == 0) {
      if (search_item != undefined) {
        console.log("render no search result");
      } else {
        renderEmptyMetricsList(viewport);
      }
      return;
    }

    var header_text = (search_item != undefined) ? 
      "Search " + search_item : "Metrics";
    FnordMetric.util.renderMetricHeader(header_text, viewport);

    var table_container = document.createElement("div");
    viewport.appendChild(table_container);

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
        var search_item;
        if (query_params != undefined) {
          json.metrics =
              actions[query_params.name].data(json.metrics, query_params.value);
          search_item = query_params.value;
        }
        renderMetricList(viewport, json.metrics, search_item);
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
