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
    if (query_params.innerView != undefined) {
      actions[query_params.innerView].render(viewport, query_params);
      return;
    }
    loadMetricList(viewport, query_params);
  };


  function onRowClick() {
    var end = Date.now();
    var start = end - 3600000;
    var raw_url = 
      "metric_list?metric="+ this.firstChild.id +
      "&view=value&start_time=" + start +
      "&end_time=" + end;

    FnordMetric.WebUI.singleton.openUrl(raw_url, true);
  };


  function renderMetricList(viewport, metrics, search_item) {
    viewport.innerHTML = "";

    if (metrics.length == 0) {
      if (search_item != undefined) {
        renderEmptySearch(viewport, search_item);
      } else {
        renderEmptyMetricsList(viewport);
      }
      return;
    }

    var header_text = (search_item != undefined) ? 
      "Metrics &rsaquo; <em>Search for &quot;" + FnordMetric.util.htmlEscape(search_item) + "&quot;</em>" :
      "Metrics &rsaquo; <em>All Metrics</em>";
    FnordMetric.util.renderPageHeader(header_text, viewport);

    var table_container = document.createElement("div");
    viewport.appendChild(table_container);

    var table_view = FnordMetric.util.TableView([
        "Metric",
        "Labels",
        "Last Insert",
        "Total stored bytes"], table_container, 25);
    console.log("foo");
    for (i in metrics) {
      table_view.addRow([
          metrics[i]["key"],
          metrics[i]["labels"].join(", "),
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

  function renderEmptySearch(elem, search_item) {
    elem.innerHTML = "";
    var header = 
      "Metrics &rsaquo; <em>No Search Results for &quot;" +
      FnordMetric.util.htmlEscape(search_item) + "&quot;</em>";
    FnordMetric.util.renderPageHeader(header,elem);

    FnordMetric.util.TableView().renderEmptyTable(
      elem, ["Metric", "Labels", "Last Insert", "Total stored bytes"]);
  }


  var destroy = function(elem) {
  }

  return {
    "render": render,
    "destroy": destroy
  };
};
