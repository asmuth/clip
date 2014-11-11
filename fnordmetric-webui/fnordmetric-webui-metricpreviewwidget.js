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

  function updateResult(inputs) {
    console.log("update result");
  }

  function updateEventHandler(elems) {
    var inputs = {
      "show" : "Value",
      "aggregation" : {
        "time" : null,
        "step" : null
        },
      "seconds" : 10,
      "date" : null,
      "group_by" : []
    }

    elems.show.addEventListener('change', function() {
      inputs.show = this.value;
      updateResult(inputs);
    }, false);

    elems.aggregation.time.addEventListener('change', function() {
      inputs.aggregation.time = this.value;
      updateResult(inputs);
    }, false);

    elems.aggregation.step.addEventListener('change', function() {
      inputs.aggregation.step = this.value;
      updateResult(inputs);
    }, false);

    elems.seconds.addEventListener('change', function() {
      inputs.seconds = this.value;
      updateResult(inputs);
    }, false);

    elems.group_by.map(function(column) {
      column.addEventListener('click', function(e) {
        e.preventDefault();
        //check if already set --> on /off
        inputs.group_by.push(this.innerText);
        updateResult(inputs);
      });
    }, false);

    //timespan & datepicker --> date



  }

  function renderElems(elem, columns) {
    var elems = {};
    var show_ttl = document.createElement("div");
    show_ttl.innerHTML = "Show";
    var show_select = document.createElement("select");
    var show_options = ["Value", "Mean", "Count", "Sum"];
    show_options.map(function(show) {
      var option = document.createElement("option");
      option.innerHTML = show;
      show_select.appendChild(option);
    });

    elems.show = show_select;

    elem.appendChild(show_ttl);
    elem.appendChild(show_select);

    var aggr_ttl = document.createElement("div");
    aggr_ttl.innerHTML = "Aggregation";
    var aggr_time = document.createElement("input");
    var aggr_step = document.createElement("input");

    elems.aggregation = {
      time : aggr_time,
      step: aggr_step
    };

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

    elems.seconds = timespan_select;

    elem.appendChild(timespan_ttl);
    elem.appendChild(timespan_select);

    var date_ttl = document.createElement("div");
    date_ttl.innerHTML = "End Date";
    var datepicker = document.createElement("input");
    elem.appendChild(date_ttl);
    elem.appendChild(datepicker);
    FnordMetric.util.DatePicker(elem, datepicker);

    if (columns.length > 0) {
      var group_buttons = [];
      columns.map(function(column) {
        var btn = FnordMetric.createButton(
          "#", undefined, column);
        group_buttons.push(btn);
        elem.appendChild(btn);
      });
      elems.group_by = group_buttons;
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

    elems.timespan = {
      prev : prev_timespan,
      next : next_timespan
    }

    elem.appendChild(timespan_updater);
    elem.appendChild(prev_timespan);
    elem.appendChild(updater_ttl);
    elem.appendChild(next_timespan);

    updateEventHandler(elems);

  }

  function render(elem, metric) {
    var columns = [];
    FnordMetric.httpGet("/metrics", function(r) {
      if (r.status == 200) {
        var json = JSON.parse(r.response);
        json.metrics.map(function(m) {
          if (m.key != metric) {return;}
          m.labels.map(function(label) {
            columns.push(label);
          });
        });
        renderElems(elem, columns);
      }
    });

  }


  return {
    "render" : render
  }

}
