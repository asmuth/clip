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
if (typeof FnordMetric == "undefined") {
  FnordMetric = {};
}

if (typeof FnordMetric.util == "undefined") {
  FnordMetric.util = {};
}

/**
  * extracts the params from the url
  * @param qstr like metric_list?metric=/osx/load_avg_15m&view=value
  */
FnordMetric.util.parseQueryString = function(qstr) {
  if (qstr == null) {return;}
  var path;
  var query_params = {};

  if (qstr.indexOf("?") >= 0) {
    path = qstr.substr(0, qstr.indexOf("?"));
    path = path.replace("#", "");

    var params_str = qstr.substr(qstr.indexOf("?") + 1);
    var raw_params = params_str.split('&');

    /* set first param which defines view's view (metric, search ...) */
    var param = raw_params[0].split('=');
    query_params.innerView = decodeURIComponent(param[0]);
    query_params.innerViewValue = decodeURIComponent(param[1]);

    for (var i = 1; i < raw_params.length; i++) {
      var param = (raw_params[i].split('=') != "undefined") ? 
        raw_params[i].split('=') : "";
      if (param[0] != "undefined") {
        query_params[decodeURIComponent(param[0])] =
           (param[1] != "undefined") ? 
           decodeURIComponent(param[1]) : "";
      }
    }

  } else {
    path = qstr != "undefined" ? qstr : "";
  }
  return {
    "path": path,
    "query_params": query_params
  };
}

/**
  * builds a querystring from the query_params, attachs it to the hash
  * and sets the url
  * @param query_params should be like query_params in parseQueryString
  * @param encode (boolean) determines if innerViewValue should be URIencoded
  * @param push_state (boolena) determines if the url should be
  * added to the browser's history
  */
FnordMetric.util.setURLQueryString = function(hash, query_params, push_state) {
  if (hash === undefined || hash === "undefined") {
    window.location.hash = "";
    return;
  }
  var path = "#" + hash;

  if ("innerView" in query_params && query_params.innerView != undefined) {
    path += "?" + encodeURIComponent(query_params.innerView) + "=";
    path +=
      encodeURIComponent(query_params.innerViewValue);

    for (var param in query_params) {
      if (param != "innerView" && 
          param != "innerViewValue" &&
          query_params[param] != undefined &&
          query_params[param].length > 0) {

        path += 
          "&" + encodeURIComponent(param) +
          "=" + encodeURIComponent(query_params[param]);
      }
    }
  }

  if (push_state) {
    window.history.pushState({url:path}, "#", path);
  }
  window.location.hash = path;
  return path;
}


/**
  * @param offset in seconds
  */
FnordMetric.util.parseTimeOffset = function(offset) {
  if (offset < 60) {
    var label = (offset == 1)? " second ago" : " seconds ago";
    return offset + label;
  } else if (offset < 3600) {
    var time = Math.floor(offset / 60);
    var label = (time == 1)? " minute ago" : " minutes ago";
    return time + label;
  } else if (offset < 86400) {
    var time =  Math.floor(offset / 3600);
    var label = (time == 1)? " hour ago" : " hours ago";
    return time + label;
  } else {
    var time = Math.floor(offset / 86400);
    var label = (time == 1)? " day ago" : " days ago";
    return time + label;
  }
}

/**
  * creates a time description like 
  * '2 hours ago - Nov 8 2014 11:33:11
  * @param timestamp unix ts in seconds, milli or microseconds
  */
FnordMetric.util.parseTimestamp = function(timestamp) {
  if (timestamp == 0) {
    return "0";
  }

  var months = [
    "Jan","Feb", "Mar","Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct","Nov", "Dec"];

  var timestamp = Math.floor(timestamp / 1000);

  var now = Date.now();
  var date = new Date(timestamp);

  var offset =  Math.floor(
    (now - timestamp) / 1000);

  var time_str = FnordMetric.util.parseTimeOffset(offset);


  var minutes = date.getMinutes();
  if (minutes < 10) {
    minutes = "0" + minutes;
  }

  var seconds = date.getSeconds();
  if (seconds < 10) {
    seconds = "0" + seconds;
  }

  time_str +=
    " - " + 
    months[date.getMonth()] + 
    " " + date.getDate() +
    " " + date.getFullYear() +
    " " + date.getHours() +
    ":" + minutes +
    ":" + seconds

  return time_str;
}

/**
  * builds a ChartSQL query from url params
  * @param params format as returned in parseQueryString
  */
FnordMetric.util.generateSQLQueryFromParams = function(params) {
  //FIX html escape 
  var table_ref = params.innerViewValue
  var view = params.view;
  /* column for rollups */
  var columns = params.columns;
  var start_time = params.start_time;
  var end_time = params.end_time;
  var t_step = params.t_step;
  var t_window = params.t_window;
  var by = params.by;

  var query;
  var draw_stm = 
    "DRAW LINECHART\n  ";
  var select_expr = "SELECT\n    time AS x,\n    ";
  var from_expr = "\n  FROM\n";
  var where_expr = "";
  var group_expr = "";
  var hasAggregation = false;

  /* complement draw_stm */
  if (!isNaN(start_time) && !isNaN(end_time)) {
    draw_stm +=
      "XDOMAIN\n    FROM_TIMESTAMP(" + start_time +
      "),\n    FROM_TIMESTAMP(" + end_time + ")";
  }

  /* complete select_expr */
  if (view == "value") {
    select_expr += "value AS y ";
  } else if (view.substr(0,6) == "rollup") {
    /* adapt draw stm */
    draw_stm = "DRAW BARCHART\n  ";
    var func = (view.split(" "))[1];

    var column;
    if (columns != undefined && (columns.split(","))[0].length > 0) {
      columns = columns.split(",")
      column = "`" + columns[0] + "`";
    } else {
      /* fallback if the metric hasn't any labels */
      column = "'total'";
    }

    select_expr = 
      " SELECT " + column + " AS X, " + func + "(value) AS Y";

    //hasAggregation = true;
  } else {
    select_expr +=
      view.toLowerCase() + "(value) AS Y";
    hasAggregation = true;
  }
  if (by != undefined && by.length > 0) {
    var series = by.replace(/,/g, " + ',' + ");
    select_expr += ", " + series + " AS series";
  }

  /* complete draw stm */
  draw_stm +=
    "\n  AXIS BOTTOM\n  AXIS LEFT\n" +
    "  LEGEND TOP RIGHT INSIDE;\n\n";


  /* complete from_expr */
  from_expr += "    `" + table_ref + "`\n";

  /*complete where_expr */
  if (!isNaN(start_time) && !isNaN(end_time)) {
    where_expr =
      "  WHERE\n    time > FROM_TIMESTAMP(" + start_time + ")\n" +
      "    AND time < FROM_TIMESTAMP(" + end_time + ")";
  }


  /*complete group_expr if an aggregate function is selected */
  if (hasAggregation) {
    group_expr = " GROUP ";
    var hasGroupStm = false;

    if (t_window != undefined) {
      hasGroupStm = true;

      group_expr += 
        "OVER TIMEWINDOW(time, " + t_window + ",";

      group_expr += (t_step != undefined)? t_step : t_window;

      group_expr+= ")";

    }

    if (by != undefined && by.length > 0) {
      hasGroupStm = true;

      group_expr += " BY " + by;
    }

    /* aggregate function without group_by statement */
    if (!hasGroupStm) {
      group_expr = "";
    }
  }


  query =
    draw_stm + select_expr + from_expr +
    where_expr + group_expr + ";";
  return query;
}

FnordMetric.util.addToCSV = function(list, value) {
  if (list.length == 0) {
    return value;
  }
  if (value.length == 0) {
    return list;
  }
  if (list.indexOf(value) > -1 ) {
    return list;
  }
  var values = list.split(",");
  values.push(value);
  list = values.join(",");
  return list;
}

FnordMetric.util.removeFromCSV = function(list, value) {
  var values = list.split(",");
  for (var i = 0; i < values.length; i ++) {
    if (values[i] == value) {
      values.splice(i, 1);
      return values.join(",");
    }
  }
  return list;
}
