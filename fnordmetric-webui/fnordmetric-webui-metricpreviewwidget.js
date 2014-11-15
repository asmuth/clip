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
  var load_pane = document.createElement("div");
  var now = Date.now();
  var columns = [];
  var elems = {};


  var defaults = {
    view : "value",
    columns: "",
    end_time : now,
    /* 5 minutes  */
    time_to_end : 300000,
    /* 1 second */
    t_step : 1000,
    t_window : 1000,
    by: ""
  }


  function getQueryParamOrDefaultValue(key) {
    var value = (query_params[key] == undefined)? 
      defaults[key] : query_params[key];

    return value;
  }

  function updateURLParams(key, value) {
    query_params[key] = value.toString();
    FnordMetric.util.setURLQueryString(
      "metric_list", query_params, false, true);
  }

  function renderChart(chart) {
    if (chart != undefined) {
      chart_container.innerHTML = chart.svg;
      FnordMetric.extendCharts();
    }
  }

  function renderTable(table) {
    var table_view = FnordMetric.util.TableView(
      table.columns, table_container, 25);
    elem.appendChild(table_container);
    for (i in table.rows) {
      table_view.addRow(table.rows[i]);
    }
    table_view.render();

  }

  function renderError(message) {
    chart_container.innerHTML = message;
  }

  function runQuery() {
    var querystr = 
      FnordMetric.util.generateSQLQueryFromParams(query_params);
    FnordMetric.util.displayLoader(chart_container);
    FnordMetric.httpPost("/query", querystr, function(r) {
      if (r.status == 200) {
        var json = JSON.parse(r.response);
        console.log(json);
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
    // change shpow to view
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
    var start = FnordMetric.util.getDateTimeString(
      getQueryParamOrDefaultValue("start_time"));
    var end = getQueryParamOrDefaultValue("end_time");
    var end_str = FnordMetric.util.getDateTimeString(end);

    if (input != null) {
      input.value = end_string;
      input.setAttribute("id", end);
    }

    title.innerHTML = 
      start + " &mdash; " + end_str;
  }

  function onDateSubmit(ts) {
    var start = 
      ts - getQueryParamOrDefault(time_to_end);
    updateURLParams("end_time", ts);
    updateURLParams("start_time", start);
    runQuery();
    updateDateTimeElems(elems.timespan_title, null);
  }

  function initElems() {
    var initial_timespan;
    var group_buttons = [];
    var end_time = getQueryParamOrDefaultValue("end_time");
    var start_time = getQueryParamOrDefaultValue("start_time");

    var controls = document.createElement("div");
    controls.className = "metric_preview_controls";
    elem.appendChild(controls);

    var rollup_group = document.createElement("div");
    rollup_group.className = "group aggregation_type";
    rollup_group.innerHTML = "<b>Rollup</b>"
    controls.appendChild(rollup_group);

    var rollup_select = document.createElement("select");
    rollup_group.appendChild(rollup_select);
    var rollup_options = 
      ["Value", "Mean", "Count", "Sum", "Rollup Sum", "Rollup Count"];
    rollup_options.map(function(rollup) {
      var option = document.createElement("option");
      option.innerHTML = rollup;
      rollup_select.appendChild(option);
    });

    rollup_select.value = 
      FnordMetric.util.reverseLowerCaseUnderscore(
        getQueryParamOrDefaultValue("view"));

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

    var datepicker = document.createElement("input");
    date_group.appendChild(datepicker);
    datepicker.setAttribute("id", end_time);
    FnordMetric.util.DatePicker(date_group, datepicker, elem, onDateSubmit);
    datepicker.value = 
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
        "1 hour"];

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
    var timespan_title = document.createElement("span");
    timespan_title.className = "current_date";
    timespan_title.innerHTML = 
      FnordMetric.util.getDateTimeString(start_time) 
      + " &mdash; " + 
      FnordMetric.util.getDateTimeString(end_time);

    secondary_controls.appendChild(timespan_updater);
    secondary_controls.appendChild(prev_timespan);
    secondary_controls.appendChild(timespan_title);
    secondary_controls.appendChild(next_timespan);

    table_container.className = "single_metric_ui table_container";
    chart_container.className = "single_metric_ui chart_container";
    elem.appendChild(chart_container);

    /* set all EventListeners */

    rollup_select.addEventListener('change', function() {
      /* queryGenerator assumes this format */
      var view = 
        FnordMetric.util.makeLowerCaseUnderscore(this.value);
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
            this.className = "selected";
            if (selected_columns.length > 0) {
              selected_columns += ",";
            }
            selected_columns += this.innerText;

          } else {
            this.className = "";
            //FIXME
            selected_columns = 
              FnordMetric.util.removeFromString(
              c, this.innerText.length+1, selected_columns);
            if (selected_columns[0] == ",") {
              selected_columns.substr(1);
            }
          }

          updateURLParams("by", selected_columns);
          runQuery();
        });
      }, false);
    }

    time_to_end.addEventListener('change', function() {
      start_time = 
        end_time - FnordMetric.util.toMilliSeconds(this.value);
      updateURLParams("start_time", start_time);
      updateDateTimeElems(timespan_title, null);
      runQuery();
    }, false);

    prev_timespan.addEventListener('click', function(e) {
      e.preventDefault();
      var end = end_time;
      end_time = start_time;
      var diff = end - start_time;
      start_time = start_time - diff;
      updateURLParams("end_time", end_time);
      updateURLParams("start_time", start_time);
      updateDateTimeElems(timespan_title, elems.date);
      runQuery();
    }, false);

    next_timespan.addEventListener('click', function(e) {
      e.preventDefault();
      var start = start_time;
      start_time = end_time;
      end_time = parseInt(end_time ,10) + (end_time - start);
      updateURLParams("start_time", start_time);
      updateURLParams("end_time", end_time);
      updateDateTimeElems(timespan_title, elems.date);
      runQuery();
    }, false);


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
        });
        initElems();
      }
    });

  }


  return {
    "render" : render
  }

}
