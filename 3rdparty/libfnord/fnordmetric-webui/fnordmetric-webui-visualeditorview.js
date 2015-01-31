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

if (FnordMetric.util === undefined) {
  FnordMetric.util = {};
}

FnordMetric.util.VisualEditorView = function() {
  function render(elem) {
    elem.innerHTML = "";
    elem.style.border = "1px solid #ddd";

    var metric_field = document.createElement("div");
    metric_field.className = "visual_editor field";
    var metric_input = document.createElement("input");
    metric_field.appendChild(metric_input);
    /* add metric dropdown */
    elem.appendChild(metric_field);

    var aggr_field = document.createElement("div");
    aggr_field.className = "visual_editor field";
    var aggr_title = document.createElement("div");
    aggr_title.innerHTML = "AGGREGATION";
    var aggr_select = document.createElement("select");
    var aggregations = ["", "SUM", "MAX", "MIN", "COUNT", "AVG"];
    aggregations.map(function(aggr) {
      var option = document.createElement("option");
      option.value = aggr;
      option.innerHTML = aggr;
      aggr_select.appendChild(option);
    });
    aggr_field.appendChild(aggr_title);
    aggr_field.appendChild(aggr_select);

    var step_title = document.createElement("div");
    step_title.innerHTML = "STEP";
    var step_input = document.createElement("input");
    step_input.className = "small";
    aggr_field.appendChild(step_title);
    aggr_field.appendChild(step_input);

    var time_title = document.createElement("div");
    time_title.innerHTML = "TIME";
    var time_input = document.createElement("input");
    time_input.className = "small right";
    aggr_field.appendChild(time_title);
    aggr_field.appendChild(time_input);
    elem.appendChild(aggr_field);

    var group_field = document.createElement("div");
    group_field.className = "visual_editor field";
  }

  function getQuery() {
    //return querystring

  }

  return {
    "render" : render,
    "getQuery" : getQuery
  }
}
