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

FnordMetric.util.MetricPreviewWidget = function() {

  function renderElems(elem, columns) {
    var show_ttl = document.createElement("div");
    show_ttl.innerHTML = "Show";
    var show_select = document.createElement("select");
    var show_options = ["Value", "Mean", "Count", "Sum"];
    show_options.map(function(show) {
      var option = document.createElement("option");
      option.innerHTML = show;
      show_select.appendChild(option);
    });
    elem.appendChild(show_ttl);
    elem.appendChild(show_select);

    var aggr_ttl = document.createElement("div");
    aggr_ttl.innerHTML = "Aggregation";
    var aggr_time = document.createElement("input");
    var aggr_step = document.createElement("input");
    elem.appendChild(aggr_ttl);
    elem.appendChild(aggr_time);
    elem.appendChild(aggr_step);

    var timespan_ttl = document.createElement("div");
    timespan_ttl.innerHTML = "Last Seconds";
    var timespan_select = document.createElement("select");
    var timespans = [10, 30, 60, 90];
    timespans.map(function(timespan) {
      var option = document.createElement("option");
      option.innerHTML = timespan;
      timespan_select.appendChild(option);
    });
    elem.appendChild(timespan_ttl);
    elem.appendChild(timespan_select);

    var date_ttl = document.createElement("div");
    date_ttl.innerHTML = "End Date";
    var datepicker = document.createElement("input");
    elem.appendChild(date_ttl);
    elem.appendChild(datepicker);

    if (columns.length > 0) {
      columns.map(function(column) {
        var btn = FnordMetric.createButton(
          "#", undefined, column);
        elem.appendChild(btn);
      });
      var group_ttl = document.createElement("div");
      group_ttl.innerHTML = "Group By";
      elem.appendChild(group_ttl);
    }

    var timespan_updater = document.createElement("div");
    var prev_timespan = FnordMetric.createButton(
      "#", undefined, "&#8592;");
    var next_timespan = FnordMetric.createButton(
      "#", undefined, "&#8594;");
    var updater_ttl = document.createElement("span");
    updater_ttl.innerHTML = "DATE";
    elem.appendChild(timespan_updater);
    elem.appendChild(prev_timespan);
    elem.appendChild(updater_ttl);
    elem.appendChild(next_timespan);

  }

  function render(elem, metric) {
    /* get columns for metric */
    columns = ["foo", "bar"];
    renderElems(elem, columns);

  }


  return {
    "render" : render
  }

}
