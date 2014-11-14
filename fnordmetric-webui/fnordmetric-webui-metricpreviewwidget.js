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

FnordMetric.util.MetricPreviewWidget = function(viewport, query_params) {
  var elem = viewport;
  var metric = query_params.innerViewValue;
  var table_container = document.createElement("div");
  var chart_container = document.createElement("div");
  var now = Date.now();
  var columns = [];
  var elems = {};
  var mseconds_to_end;

  var defaults = {
    view : "value",
    column: null,
    end_time : now,
    /* 5 minutes  */
    time_to_end : 300000,
    /* 1 second */
    t_step : 1000,
    t_window : 1000,
    by : columns.join(", ")
  }


  function generateSQLQuery() {
    return FnordMetric.util.generateSQLQueryFromParams(query_params);
  }



  function getQueryParamOrDefaultValue(key) {
    var value = (query_params[key] == undefined)? 
      defaults[key] : query_params[key];

    return value;
  }

  function updateURLParams(key, value) {
    query_params[key] = value;
    //update url fragment
  }

  function renderChart(chart) {
    if (chart != undefined) {
      chart_container.innerHTML = "";
      chart_container.className = "single_metric_ui chart_container";
      chart_container.innerHTML = chart.svg;
      elem.appendChild(chart_container);
      FnordMetric.extendCharts();
    }
  }

  function renderTable(table) {
    table_container.innerHTML = "";
    var table_view = FnordMetric.util.TableView(
      table.columns, table_container, 25);
    elem.appendChild(table_container);
    for (i in table.rows) {
      table_view.addRow(table.rows[i]);
    }
    table_view.render();

  }

  function runQuery() {
    var querystr = 
      FnordMetric.util.generateSQLQueryFromParams(query_params);
    FnordMetric.httpPost("/query", querystr, function(r) {
      if (r.status == 200) {
        var json = JSON.parse(r.response);
        if (json.charts != undefined) {
          renderChart(json.charts[0]);
        }
        if (json.tables != undefined) {
          renderTable(json.tables[0]);
        }
      }
    });
  }


  function handleAggrAvailability(show, tw_select, step_select, group_btns) {
    /* show = "value" */
    var group_btns = (group_btns == undefined)? [] : group_btns;
    if (show == "Value" || show == "Rollup") {
      tw_select.className = "disabled";
      tw_select.disabled = true;
      step_select.className = "disabled";
      step_select.disabled = true;
      if (show == "Value") {
        group_btns.map(function(btn) {
          btn.className = "disabled";
        });
      } else {
        group_btns.map(function(btn) {
          btn.className = "";
        });
      }
    } else {
      tw_select.className = "";
      tw_select.disabled = false;
      step_select.className = "";
      step_select.disabled = false;
      group_btns.map(function(btn) {
        btn.className = "";
      });
    }

  }

  function updateDateTimeElems(title, input) {
    var start = FnordMetric.util.getdateTimeString(
      getQueryParamOrDefault("start_time"));
    var end = getQueryParameOrDefault("end_time");
    var end_str = FnordMetric.util.getDateTimeString(end);

    if (input != null) {
      input.value = end_string;
      input.setAttribute("id", end);
    }

    title.innerHTML = 
      FnordMetric.util.getDateTimeString(start) +
      " &mdash; " + end_string;

  }

  function onDateSubmit(ts) {
    var start = 
      ts - getQueryParamOrDefault(time_to_end);
    updateURLParams("end_time", ts);
    updateURLParams("start_time", start);
    runQuery();
    updateDateTimeElems(elems.timespan_title, null);
  }


  function updateEventHandler() {
    elems.rollup.addEventListener('change', function() {
      updateURLParams("show", this.value);
      handleAggrAvailability(
        this.value, elems_timewindow, elems.t_step, elems.by);
      runQuery();
    }, false);

    elems.t_window.addEventListener('change', function() {
      updateURLParams(
        "t_window", FnordMetric.util.toMilliSeconds(this.value));
      runQuery();
    }, false);

    elems.t_step.addEventListener('change', function() {
      updateURLParams(
        "t_step", FnordMetric.util.toMilliSeconds(this.value));
      runQuery();
    }, false);

    elems.time_to_end.addEventListener('change', function() {
      mseconds_to_end = 
        FnordMetric.util.toMilliSeconds(this.value);
      var start = getQueryParamOrDefault("end_time") - mseconds_to_end;
      updateURLParams("start_time", start);
      updateDateTimeElems(elems.timespan_title, null);
      runQuery();
    }, false);

    if (elems.by) {
      elems.by.map(function(column) {
        column.addEventListener('click', function(e) {
          e.preventDefault();
          var c = this.innerText;
          var index = inputs.group_by.indexOf(c);
          if (index == -1) {
            this.className = "selected";
            updateURLParams(this.innerText);
          } else {
            this.className = "";
            inputs.group_by.splice(index, 1);
          }
          runQuery();
        });
      }, false);
    }

    elems.timespan_next.addEventListener('click', function(e) {
      e.preventDefault();
      var end_time =
        parseInt(getQueryParamOrDefault("end_time", 10) +
        parseInt(mseconds_to_end, 10));

      updateURLParams("end_time", end_time);
      //upfdate start_time
      updateDateTimeElems(elems.timespan_title, elems.date);
      runQuery();
    }, false);

    elems.timespan_prev.addEventListener('click', function(e) {
      e.preventDefault();
      var end_time = 
        parseInt(getQueryParamOrDefault("end_time", 10) -
        parseInt(mseconds_to_end, 10));
      updateURLParams("ent_time", end_time);
      //update start time
      updateDateTimeElems(elems.timespan_title, elems.date);
      runQuery();
    }, false);

  }


  function initElems() {
    var initial_timespan;

    var controls = document.createElement("div");
    controls.className = "metric_preview_controls";
    elem.appendChild(controls);

    var rollup_group = document.createElement("div");
    rollup_group.className = "group aggregation_type";
    rollup_group.innerHTML = "<b>Rollup</b>"
    controls.appendChild(rollup_group);

    var rollup_select = document.createElement("select");
    rollup_group.appendChild(rollup_select);
    var rollup_options = ["Value", "Mean", "Count", "Sum", "Rollup"];
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

    var timewindow = document.createElement("div");
    timewindow.className = "group aggregation_time_window";
    timewindow.innerHTML = "<b>Time Window / Step<b>";
    controls.appendChild(timewindow);

    var t_window = document.createElement("select");
    timewindow.appendChild(t_window);
    aggregate_options.map(function(opt) {
      var option = document.createElement("option");
      option.innerHTML = opt;
      t_window.appendChild(option);
    });

    var t_step = document.createElement("select");
    timewindow.appendChild(t_step);
    aggregate_options.map(function(opt) {
      var option = document.createElement("option");
      option.innerHTML = opt;
      t_step.appendChild(option);
    });

    /*elems.aggregation = {
      "timewindow" : aggr_win,
      "step": aggr_step
    };*/

    elems.t_window = t_window;
    elems.t_step = t_step;

    var date_group = document.createElement("div");
    date_group.innerHTML = "<b>End Time<b>"
    date_group.className = "group date";
    controls.appendChild(date_group);

    var datepicker = document.createElement("input");
    date_group.appendChild(datepicker);
    datepicker.setAttribute("id", now);
    FnordMetric.util.DatePicker(date_group, datepicker, elem, onDateSubmit);
    datepicker.value = FnordMetric.util.getDateTimeString(now);
    elems.date = datepicker;

    var timespan_group = document.createElement("div");
    timespan_group.className = "group timespan";
    controls.appendChild(timespan_group);

    var time_to_end_ttl = document.createElement("b");
    time_to_end_ttl.innerHTML = "Show the last...";
    var time_to_end = document.createElement("select");
    var timespans = [
        "5 minutes",
        "15 minutes",
        "30 minutes",
        "1 hour"];

    timespans.map(function(timespan) {
      var option = document.createElement("option");
      option.innerHTML = timespan;
      time_to_end.appendChild(option);
    });
    //update
    initial_timespan = FnordMetric.util.toMilliSeconds(
      timespans[0]);

    elems.time_to_end = time_to_end;
    timespan_group.appendChild(time_to_end_ttl);
    timespan_group.appendChild(time_to_end);
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
      elems.by = group_buttons;
    }

    var secondary_controls = document.createElement("div");
    elem.appendChild(secondary_controls);
    secondary_controls.className = "metric_preview_secondary_controls";

    var controls_query = FnordMetric.createButton(
      "#", "btn", "<i class='fa fa-database'></i> SQL Editor");
    controls_query.onclick = function(e) {
      e.preventDefault();
      var query = 
        decodeURIComponent("SELECT * FROM " + metric);
      FnordMetric.WebUI.singleton.openUrl(
        "query_playground?sql_query="+query, true);
    };

    var controls_embed = FnordMetric.createButton(
      "#", "btn", "<i class='fa fa-share'></i> Embed");
    controls_embed.onclick = function(e) {
      e.preventDefault();
      FnordMetric.util.openPopup(
        elem, "TODO add embed snippet");
    }

    secondary_controls.appendChild(controls_query);
    secondary_controls.appendChild(controls_embed);

    var timespan_updater = document.createElement("div");
    var prev_timespan = FnordMetric.createButton(
      "#", undefined, "<i class='fa fa-chevron-left'></i>");
    var next_timespan = FnordMetric.createButton(
      "#", undefined, "<i class='fa fa-chevron-right'></i>");
    var updater_ttl = document.createElement("span");
    updater_ttl.className = "current_date";
    updater_ttl.innerHTML = 
      FnordMetric.util.getDateTimeString(
        now - initial_timespan) + " &mdash; " + 
      FnordMetric.util.getDateTimeString(now);

    /*elems.timespan = {
      prev : prev_timespan,
      next : next_timespan,
      title: updater_ttl
    }*/

    elems.timespan_next = next_timespan;
    elems.timespan_prev = prev_timespan;
    elems.timespan_title = updater_ttl;

    secondary_controls.appendChild(timespan_updater);
    secondary_controls.appendChild(prev_timespan);
    secondary_controls.appendChild(updater_ttl);
    secondary_controls.appendChild(next_timespan);

    updateEventHandler();

    handleAggrAvailability("Value", t_window, t_step, group_buttons);

    runQuery(generateSQLQuery());
  }

  function render() {
    FnordMetric.httpGet("/metrics", function(r) {
      if (r.status == 200) {
        var json = JSON.parse(r.response);
        json.metrics.map(function(m) {
          if (m.key != metric) {return;}
          m.labels.map(function(label) {
            columns.push(label);
          });
        });
        initElems();
      }
    });

  }


  return {
    "render" : render
  }

}
