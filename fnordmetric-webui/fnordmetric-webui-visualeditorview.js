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
    aggr_field.innerHTML = "AGGREGATION";
    var aggr_select = document.createElement("select");
    var aggregations = ["", "SUM", "MAX", "MIN", "COUNT", "AVG"];
    aggregations.map(function(aggr) {
      var option = document.createElement("option");
      option.value = aggr;
      option.innerHTML = aggr;
      aggr_select.appendChild(option);
    });
    aggr_field.appendChild(aggr_select);
    elem.appendChild(aggr_field);



  }
  return {
    "render" : render
  }
}
