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
  var controls_query;
  var auto_refresh = FnordMetric.util.autoRefresh(
    onRefresh);

  var datetime_title;
  var datetime_input;

  var defaults = {
    view : "value",
    columns: "",
    end_time : now,
    /* 5 minutes  */
    time_to_end : 3600000,
    /* 1 second */
    t_step : 10000,
    t_window : 5000,
    by: ""
  }


  function getQueryParamOrDefaultValue(key) {
    var value = (query_params[key] == undefined)? 
      defaults[key] : query_params[key];

    return value;
  }

  // FIXLAURA allow multiple key value pairs as input
  function updateURLParams(key, value) {
    if (value != undefined) {
      query_params[key] = value.toString();
      FnordMetric.util.setURLQueryString(
        "metric_list", query_params, false, true);
    }
  }

  /* checks if required url params are misssing and adds those if so */
  function addRequiredURLParamsForView(value) {
    if (value == "count" || 
        value == "sum" || 
        value == "mean" ||
        value == "min" ||
        value == "max") {

      var time_window = query_params.t_window;
      if (time_window == undefined) {
        time_window = defaults.t_window;
        updateURLParams("t_window", time_window);
      }
    }
    var group_by = query_params.by;
    if (group_by == undefined) {
      group_by = defaults.by;
      updateURLParams("by", group_by);
    }
    var param_columns = query_params.columns;
    if (param_columns == undefined) {
      param_columns = defaults.columns;
      updateURLParams("columns", param_columns);
    }
  }

  function renderChart(chart) {
    if (chart != undefined) {
      chart_container.innerHTML = chart.svg;
      FnordMetric.extendCharts();
    }
  }

  function renderTable(table) {
    var table_view = FnordMetric.util.TableView(
      table.columns, table_container, 25, "bottom");
    elem.appendChild(table_container);
    for (i in table.rows) {
      table_view.addRow(table.rows[i]);
    }
    table_view.render();

  }

  function renderError(message) {
    var error_box = document.createElement("div");
    error_box.className = "error_box";
    error_box.innerHTML = message;
    chart_container.innerHTML = "";
    chart_container.appendChild(error_box);
  }

  function updateSQLQueryLink(query) {
    controls_query.href =
      "/admin#query_playground?sql_query=" +
      encodeURIComponent(query);
  }

  function runQuery() {
    var querystr =
      FnordMetric.util.generateSQLQueryFromParams(query_params);
    updateSQLQueryLink(querystr);
    var queryurl = "/query?height=400&width=" + (window.innerWidth - 40);
    FnordMetric.util.displayLoader(chart_container);
    table_container.innerHTML = "";
    FnordMetric.httpPost(queryurl, querystr, function(r) {
      if (r.status == 200) {
        var json = JSON.parse(r.response);
        if (json.charts != undefined) {
          renderChart(json.charts[0]);
        } else {
          renderError(json.error);
        }
        if (json.tables != undefined) {
          renderTable(json.tables[0]);
        }
      }
    });
  }


  function handleAggregationDisplay(show, tw_select, step_select, group_btns) {
    var group_btns = (group_btns == undefined)? [] : group_btns;
    if (show == "Value" || show.substr(0,6) == "Rollup") {
      tw_select.className = "disabled";
      tw_select.disabled = true;
      step_select.className = "disabled";
      step_select.disabled = true;
    } else {
      tw_select.className = "";
      tw_select.disabled = false;
      step_select.className = "";
      step_select.disabled = false;
    }
    var by_str = (query_params.by == undefined)?
      "" : query_params.by;
    group_btns.map(function(btn) {
      if (by_str.indexOf(btn.innerText) > -1) {
        btn.className = "active";
      } else {
        btn.className = "";
      }
    });
  }

  function updateDateTimeElems(start_time, end_time) {
    var start_time = (start_time !== undefined) ? 
       start_time : getQueryParamOrDefaultValue("start_time");
    var start_str = 
      FnordMetric.util.getDateTimeString(start_time);

    var end_time = (end_time !== undefined) ?
      end_time : getQueryParamOrDefaultValue("end_time");
    var end_str = FnordMetric.util.getDateTimeString(end_time);

    datetime_input.value = end_str;
    datetime_input.setAttribute("id", end_time);

    datetime_title.innerHTML = 
      start_str + " &mdash; " + end_str;
  }


  function onDateSubmit(timestamp) {
    auto_refresh.off();
    var timestamp = parseInt(timestamp, 10);
    var start_time = 
      parseInt(getQueryParamOrDefaultValue("start_time"), 10);
    var end_time = 
      parseInt(getQueryParamOrDefaultValue("end_time"), 10);

    start_time = timestamp - (end_time - start_time);
    updateURLParams("end_time", timestamp);
    updateURLParams("start_time", start_time);

    runQuery();
    updateDateTimeElems(start_time, timestamp);
  }

  function onRefresh() {
    var end_time = Date.now();
    var timediff = parseInt(query_params.end_time) -
      parseInt(query_params.start_time);
    var start_time = end_time - timediff;
    updateURLParams("end_time", end_time);
    updateURLParams("start_time", start_time);
    updateDateTimeElems(start_time, end_time);
    runQuery();
  }


  function initElems() {
    var initial_timespan;
    var group_buttons = [];
    var end_time = getQueryParamOrDefaultValue("end_time");
    var start_time = getQueryParamOrDefaultValue("start_time");

    /***** primary controls *****/

    var controls = document.createElement("div");
    controls.className = "metric_preview_controls";
    elem.appendChild(controls);

    var rollup_group = document.createElement("div");
    rollup_group.className = "group aggregation_type";
    rollup_group.innerHTML = "<b>Rollup</b>"
    controls.appendChild(rollup_group);

    var rollup_select = document.createElement("select");
    rollup_group.appendChild(rollup_select);

    var rollup_options = [
      "Value",
      "Sum",
      "Count",
      "Min",
      "Max",
      "Mean",
      "Median",
      "Rollup Mean",
      "Rollup Count",
      "Rollup Sum"];

    rollup_options.map(function(rollup) {
      var option = document.createElement("option");
      option.innerHTML = rollup;
      rollup_select.appendChild(option);
    });

    var rollup_value = 
      FnordMetric.util.reverseLowerCaseUnderscore(
        getQueryParamOrDefaultValue("view"));

    rollup_select.value = rollup_value;

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

    t_window.value = 
      FnordMetric.util.milliSecondsToTimeString(
        getQueryParamOrDefaultValue("t_window"));

    var t_step = document.createElement("select");
    timewindow.appendChild(t_step);
    aggregate_options.map(function(opt) {
      var option = document.createElement("option");
      option.innerHTML = opt;
      t_step.appendChild(option);
    });

    t_step.value = 
      FnordMetric.util.milliSecondsToTimeString(
        getQueryParamOrDefaultValue("t_step"));

    var date_group = document.createElement("div");
    date_group.innerHTML = "<b>End Time<b>"
    date_group.className = "group date";
    controls.appendChild(date_group);

    datetime_input = document.createElement("input");
    date_group.appendChild(datetime_input);
    datetime_input.setAttribute("id", end_time);
    FnordMetric.util.DatePicker(date_group, datetime_input, elem, onDateSubmit);
    datetime_input.value = 
      FnordMetric.util.getDateTimeString(end_time);

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
        "1 hour",
        "4 hours",
        "12 hours",
        "1 day"];

    timespans.map(function(timespan) {
      var option = document.createElement("option");
      option.innerHTML = timespan;
      time_to_end.appendChild(option);
    });

    time_to_end.value =
      FnordMetric.util.parseMilliTS(end_time - start_time);

    timespan_group.appendChild(time_to_end_ttl);
    timespan_group.appendChild(time_to_end);
    controls.appendChild(timespan_group);

    var groupby_group = document.createElement("div");
    groupby_group.className = "group groupby";
    groupby_group.innerHTML = "<b>Group By</b>";
    controls.appendChild(groupby_group);

    if (columns.length > 0) {
      columns.map(function(column) {
        var btn = FnordMetric.createButton(
          "#", undefined, "<i class='fa fa-toggle-off'></i>" + column);
        group_buttons.push(btn);
        groupby_group.appendChild(btn);
      });
    }

    /****** secondary controls ******/

    var secondary_controls = document.createElement("div");
    elem.appendChild(secondary_controls);
    secondary_controls.className = "metric_preview_secondary_controls";

    controls_query = FnordMetric.createButton(
      "#", "btn", "<i class='fa fa-database'></i> SQL Editor");

    var controls_embed = FnordMetric.createButton(
      "#", "btn", "<i class='fa fa-share'></i> Embed");

    var controls_autorefresh = FnordMetric.createButton(
      "#", "btn", "<i class='fa fa-refresh'></i> Auto Refresh");
    auto_refresh.btn = controls_autorefresh;

    secondary_controls.appendChild(controls_query);
    secondary_controls.appendChild(controls_embed);
    secondary_controls.appendChild(controls_autorefresh);

    var timespan_updater = document.createElement("div");
    var prev_timespan = FnordMetric.createButton(
      "#", undefined, "<i class='fa fa-chevron-left'></i>");
    var next_timespan = FnordMetric.createButton(
      "#", undefined, "<i class='fa fa-chevron-right'></i>");
    datetime_title = document.createElement("span");
    datetime_title.className = "current_date";
    datetime_title.innerHTML = 
      FnordMetric.util.getDateTimeString(start_time) 
      + " &mdash; " + 
      FnordMetric.util.getDateTimeString(end_time);

    secondary_controls.appendChild(timespan_updater);
    secondary_controls.appendChild(prev_timespan);
    secondary_controls.appendChild(datetime_title);
    secondary_controls.appendChild(next_timespan);

    table_container.className = "single_metric_ui table_container";
    chart_container.className = "single_metric_ui chart_container";
    elem.appendChild(chart_container);

    /****** set all EventListeners ******/

    rollup_select.addEventListener('change', function() {
      /* queryGenerator assumes this format */
      var view = 
        FnordMetric.util.makeLowerCaseUnderscore(this.value);
      addRequiredURLParamsForView(view);
      updateURLParams("view", view);
      handleAggregationDisplay(
        this.value, t_window, t_step, group_buttons);
      runQuery();
    }, false);

    t_window.addEventListener('change', function() {
      updateURLParams(
        "t_window", FnordMetric.util.toMilliSeconds(this.value));
      runQuery();
    }, false);

    t_step.addEventListener('change', function() {
      updateURLParams(
        "t_step", FnordMetric.util.toMilliSeconds(this.value));
      runQuery();
    }, false);

    /* update group_by columns: remove if already set and set otherwise */
    if (group_buttons.length > 0) {
      group_buttons.map(function(column) {
        column.addEventListener('click', function(e) {
          e.preventDefault();
          var selected_columns =
            getQueryParamOrDefaultValue("by");
          var c = this.innerText;
          var index = selected_columns.indexOf(c);
          if (index == -1) {
            this.className = "active";
            selected_columns =
              FnordMetric.util.addToCSV(selected_columns, c);
          } else {
            this.className = "";
            selected_columns =
              FnordMetric.util.removeFromCSV(selected_columns, c);
          }
          updateURLParams("by", selected_columns);
          runQuery();
        });
      }, false);
    }

    controls_embed.addEventListener('click', function(e) {
      e.preventDefault();
      FnordMetric.util.embedPopup(
          elem,
          FnordMetric.util.generateSQLQueryFromParams(query_params)).render();
    }, false);

    controls_query.addEventListener('click', function(e) {
      e.preventDefault();
      var query = encodeURIComponent(
        FnordMetric.util.generateSQLQueryFromParams(
          query_params));
      FnordMetric.WebUI.singleton.openUrl(
        "query_playground?sql_query="+query, true);
    }, false);

    controls_autorefresh.addEventListener('click', function(e) {
      e.preventDefault();
      if (auto_refresh.state()) {
        auto_refresh.off();
      } else {
        auto_refresh.on();
      }
    }, false);


    time_to_end.addEventListener('change', function() {
      start_time = 
        end_time - FnordMetric.util.toMilliSeconds(this.value);
      updateURLParams("start_time", start_time);
      updateDateTimeElems(start_time, end_time);
      runQuery();
    }, false);

    prev_timespan.addEventListener('click', function(e) {
      e.preventDefault();
      auto_refresh.off();
      var end = query_params.end_time;
      end_time = query_params.start_time;
      var diff = end - query_params.start_time;
      start_time = query_params.start_time - diff;
      updateURLParams("end_time", end_time);
      updateURLParams("start_time", start_time);
      updateDateTimeElems(start_time, end_time);
      runQuery();
    }, false);

    next_timespan.addEventListener('click', function(e) {
      e.preventDefault();
      auto_refresh.off();
      var start = query_params.start_time;
      var end =
        parseInt(query_params.end_time ,10) + 
        (query_params.end_time - start);

      if (end <= now) {
        start_time = query_params.end_time;
        end_time = end;
        updateURLParams("start_time", start_time);
        updateURLParams("end_time", end_time);
        updateDateTimeElems(start_time, end_time);
        runQuery();
      }
    }, false);


    addRequiredURLParamsForView(rollup_value);
    handleAggregationDisplay(rollup_select.value, t_window, t_step, group_buttons);
    runQuery();
  }

  function render() {
    FnordMetric.httpGet("/metrics", function(r) {
      if (r.status == 200) {
        var json = JSON.parse(r.response);
        json.metrics.map(function(m) {
          //FIXME what's a better way to get the 
          //metrics columns without searching through all metrics
          if (m.key != metric) {return;}
          m.labels.map(function(label) {
            columns.push(label);
          });
          query_params.columns = m.labels.join(",");
          defaults.columns = m.labels.join(",");
          defaults.by = defaults.columns;
        });
        initElems();
      }
    });

  }


  return {
    "render" : render
  }

}
