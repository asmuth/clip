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

FnordMetric.util.MetricPreviewWidget = function(viewport, metric) {
  var elem = viewport;
  var metric = metric;
  var table_container = document.createElement("div");
  var chart_container = document.createElement("div");

  function renderChart(chart) {
    if (chart != undefined) {
      chart_container.innerHTML = "";
      chart_container.className = "single_metric_ui chart_container";
      chart_container.innerHTML = chart.svg;
      elem.appendChild(chart_container);
    }
  }

  function renderTable(table) {
    table_container.innerHTML = "";
    var table_view = FnordMetric.util.TableView(
      ["series", "x", "y"], table_container, 25);
    elem.appendChild(table_container);
    for (i in table.rows) {
      table_view.addRow(table.rows[i]);
    }
    table_view.render();

  }

  function runQuery(querystr) {
    console.log(querystr);
    FnordMetric.httpPost("/query", querystr, function(r) {
      if (r.status == 200) {
        var json = JSON.parse(r.response);
        console.log(json);
        if (json.charts != undefined) {
          renderChart(json.charts[0]);
        }
        if (json.tables != undefined) {
          renderTable(json.tables[0]);
        }
      }
    });
  }

  //FIXME works but seems to be ugly
  function updateEventHandler(elems) {
    var inputs = {
      "show" : null,
      "aggregation" : {
        "time" : null,
        "step" : null
        },
      "time" : {
        "start" : null,
        "end" : null
      },
      "group_by" : []
    }

    elems.rollup.addEventListener('change', function() {
      var value = (this.value == "Value") ? null : this.value;
      inputs.show = value;
      runQuery(FnordMetric.util.createQuery(inputs, metric));
    }, false);

    elems.aggregation.time.addEventListener('change', function() {
      inputs.aggregation.time = this.value;
      runQuery(FnordMetric.util.createQuery(inputs, metric));
    }, false);

    elems.aggregation.step.addEventListener('change', function() {
      inputs.aggregation.step = this.value;
      runQuery(FnordMetric.util.createQuery(inputs, metric));
    }, false);

    elems.seconds.addEventListener('change', function() {
      inputs.time.start = inputs.time.end - (this.value * 1000);
      runQuery(FnordMetric.util.createQuery(inputs, metric));
    }, false);

    elems.group_by.map(function(column) {
      column.addEventListener('click', function(e) {
        e.preventDefault();
        var c = this.innerText;
        var index = inputs.group_by.indexOf(c);
        if (index == -1) {
          inputs.group_by.push(this.innerText);
        } else {
          inputs.group_by.splice(index, 1);
        }
        runQuery(FnordMetric.util.createQuery(inputs, metric));
      });
    }, false);

    elems.date.addEventListener('change', function() {
      //inputs.time.start = 
      runQuery(FnordMetric.util.createQuery(inputs, metric));
    }, false);

    //last_ seconds & timespan & datepicker --> date
  }

  function initElems(columns) {
    var elems = {};

    var controls = document.createElement("div");
    controls.className = "metric_preview_controls";
    elem.appendChild(controls);

    var rollup_group = document.createElement("div");
    rollup_group.className = "group aggregation_type";
    rollup_group.innerHTML = "<b>Rollup</b>"
    controls.appendChild(rollup_group);

    var rollup_select = document.createElement("select");
    rollup_group.appendChild(rollup_select);
    var rollup_options = ["Value", "Mean", "Count", "Sum"];
    rollup_options.map(function(rollup) {
      var option = document.createElement("option");
      option.innerHTML = rollup;
      rollup_select.appendChild(option);
    });
    elems.rollup = rollup_select;

    var aggregate_options = [
        "1s",
        "5s",
        "10s",
        "20s",
        "30s",
        "1m",
        "5m",
        "15m",
        "30m",
        "1h",
        "2h",
        "6h",
        "12h",
        "24h"];

    var aggr_group = document.createElement("div");
    aggr_group.className = "group aggregation_time_window";
    aggr_group.innerHTML = "<b>Time Window / Step<b>";
    controls.appendChild(aggr_group);

    var aggr_win = document.createElement("select");
    aggr_group.appendChild(aggr_win);
    aggregate_options.map(function(opt) {
      var option = document.createElement("option");
      option.innerHTML = opt;
      aggr_win.appendChild(option);
    });

    var aggr_step = document.createElement("select");
    aggr_group.appendChild(aggr_step);
    aggregate_options.map(function(opt) {
      var option = document.createElement("option");
      option.innerHTML = opt;
      aggr_step.appendChild(option);
    });

    elems.aggregation = {
      "window" : aggr_win,
      "step": aggr_step
    };

    var date_group = document.createElement("div");
    date_group.className = "group date";
    controls.appendChild(date_group);

    var date_ttl = document.createElement("b");
    date_ttl.innerHTML = "End Date";
    var datepicker = document.createElement("input");
    date_group.appendChild(date_ttl);
    date_group.appendChild(datepicker);
    FnordMetric.util.DatePicker(elem, datepicker);

    var timespan_group = document.createElement("div");
    timespan_group.className = "group timespan";
    controls.appendChild(timespan_group);

    var timespan_ttl = document.createElement("b");
    timespan_ttl.innerHTML = "Show the last...";
    var timespan_select = document.createElement("select");
    var timespans = [
        "5 minutes",
        "15 minutes",
        "30 minutes",
        "1 hour"];

    timespans.map(function(timespan) {
      var option = document.createElement("option");
      option.innerHTML = timespan;
      timespan_select.appendChild(option);
    });

    elems.seconds = timespan_select;
    timespan_group.appendChild(timespan_ttl);
    timespan_group.appendChild(timespan_select);
    controls.appendChild(timespan_group);

    var groupby_group = document.createElement("div");
    groupby_group.className = "group groupby";
    groupby_group.innerHTML = "<b>Group By</b>";
    controls.appendChild(groupby_group);

    if (columns.length > 0) {
      var group_buttons = [];
      columns.map(function(column) {
        var btn = FnordMetric.createButton(
          "#", undefined, "<i class='fa fa-toggle-off'></i>" + column);
        group_buttons.push(btn);
        groupby_group.appendChild(btn);
      });
      elems.group_by = group_buttons;
    }

    var secondary_controls = document.createElement("div");
    elem.appendChild(secondary_controls);
    secondary_controls.className = "metric_preview_secondary_controls";
    secondary_controls.innerHTML = "<div class='btn'><i class='fa fa-database'></i> SQL Editor</div>";
    secondary_controls.innerHTML += "<div class='btn'><i class='fa fa-share'></i> Embed</div>";

    var timespan_updater = document.createElement("div");
    var prev_timespan = FnordMetric.createButton(
      "#", undefined, "<i class='fa fa-chevron-left'></i>");
    var next_timespan = FnordMetric.createButton(
      "#", undefined, "<i class='fa fa-chevron-right'></i>");
    var updater_ttl = document.createElement("span");
    updater_ttl.className = "current_date";
    updater_ttl.innerHTML = "2014-11-11 20:15 &mdash; now";

    elems.timespan = {
      prev : prev_timespan,
      next : next_timespan
    }

    secondary_controls.appendChild(timespan_updater);
    secondary_controls.appendChild(prev_timespan);
    secondary_controls.appendChild(updater_ttl);
    secondary_controls.appendChild(next_timespan);

    updateEventHandler(elems);

    var end_time = FnordMetric.util.humanDateToMikroTS(
      datepicker.value);
    var start_time = end_time - (timespan_select.value * 1000);

    runQuery(FnordMetric.util.createQuery({
      "show" : null,
      "aggregation" : {
        "time" : null,
        "step" : null
        },
      "time" : {
        "start" : start_time,
        "end" : end_time
      },
      "group_by" : []
    }, metric));


  }

  function render() {
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
        initElems(columns);
      }
    });

  }


  return {
    "render" : render
  }

}
