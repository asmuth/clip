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

/**
  * extracts the params from the url
  * @param qstr like metric_list?metric=/osx/load_avg_15m&view=value
  */
FnordMetric.util.parseQueryString = function(qstr) {
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
FnordMetric.util.setURLQueryString = function(hash, query_params, encode, push_state) {
  if (hash === undefined || hash === "undefined") {
    window.location.hash = "";
    return;
  }
  var path = "#" + hash;

  if ("innerView" in query_params && query_params.innerView != undefined) {
    path += "?" + query_params.innerView + "=";
    path += (encode)?
      encodeURIComponent(query_params.innerViewValue) :
      query_params.innerViewValue;

    for (var param in query_params) {
      if (param != "innerView" && 
          param != "innerViewValue" &&
          query_params[param] != undefined &&
          query_params[param].length > 0) {

        path += 
          "&" + param +
          "=" + query_params[param];
      }
    }
  }

  if (push_state) {
    console.log("push state");
    window.history.pushState({url:path}, "#", path);
  }
  window.location.hash = path;
}



/* simple loader foreground */
FnordMetric.util.displayLoader = function(elem) {
  elem.innerHTML = "<div class='load_foreground'><i class='fa fa-refresh fa-spin'></div>";
}

/*
 * loader foreground if loader can't be 
 * destroyed with resetting the innerHTML 
*/
FnordMetric.util.Loader = function() {
  var loader  = document.createElement("div");
  loader.className = "load_foreground";
  loader.innerHTML = 
    "<i class = 'fa fa-refresh fa-spin'>";
  on_click = null;

  function onClick(on_click_new) {
    on_click = on_click_new;
  }

  function display(elem) {
    elem.appendChild(loader);
    if (on_click != null) {
      loader.onclick = on_click;
    }
  }

  function destroy(elem) {
    //FIXME
    loader = elem.querySelector(".load_foreground");
    elem.removeChild(loader);
  }

  return {
    "display" : display,
    "destroy" : destroy,
    "onClick" : onClick
  }
}


FnordMetric.util.displayErrorMessage = function(elem, msg) {
  elem.innerHTML = "<div>" + msg + "</div>"; // XSS!
}


FnordMetric.util.renderPageHeader = function(text, elem) {
  var header = document.createElement("h1");
  header.className = "page_header";
  header.innerHTML = text;

  elem.appendChild(header);
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

FnordMetric.util.getHumanMonth = function(index, type) {
  var months = {
    "long" : [
      "January",
      "February",
      "March",
      "April",
      "May",
      "June",
      "July",
      "August",
      "September",
      "October",
      "November",
      "December"],
    "short" : [
      "Jan",
      "Feb",
      "Mar",
      "Apr",
      "May",
      "Jun",
      "Jul",
      "Aug",
      "Sep",
      "Oct",
      "Nov",
      "Dec"]};

  return months[type][index];
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
  var timestamp =
    FnordMetric.util.convertToMilliTS(timestamp);

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
    FnordMetric.util.getHumanMonth(date.getMonth(), "short") + 
    " " + date.getDate() +
    " " + date.getFullYear() +
    " " + date.getHours() +
    ":" + minutes +
    ":" + seconds

  return time_str;
}

//FIXLAURA check all cases 
FnordMetric.util.convertToMilliTS = function(ts) {
  var length = ts.toString().length;
  if (length == 16) {
    return (ts/1000);
  } else if (length < 13 && length >= 10) {
    return (ts * 1000);
  } else {
    return ts;
  }
}

FnordMetric.util.parseMilliTS = function(ts) {
  var ts = FnordMetric.util.convertToMilliTS(ts);
  if (ts < 1000) {
    if (ts == 0) {
      return " less than 1 millisecond";
    }
    if (ts == 1) {
      return " 1 millisecond";
    }
    return ts + " milliseconds";
  }

  if (ts < 60000) {
    ts = ts / 1000;
    return (ts + (ts == 1? " second" : " seconds"));
  }

  if (ts < 3600000){
    ts = ts / 60000;
    return (ts + (ts == 1? " minute" : " minutes"));
  }

  ts = ts / 3600000;
  return (ts + (ts == 1? " hour" : " hours"));
}

FnordMetric.util.humanCountRows = function(tables) {
  if (tables == undefined) {
    return "0 rows";
  }

  var num = 0;
  tables.map(function(table) {
    num += table.rows.length;
  });
  return (num == 1? num + " row" : num + " rows")
}

/**
  * sorts the metric list for a specific column 
  * @param metrics is an array of arrays
  * @param column_index determines which 'column'
  *   ar array index should be sorted
  * @param order can be asc or desc
  */
FnordMetric.util.sortMetricList = function(metrics, column_index, order) {
  function compare(a, b) {
    if (a < b) {
      if (order == "asc") {
        return -1;
      } else {
        return 1;
      }
    }
    if (a > b) {
      if (order == "asc") {
        return 1;
      } else {
        return -1;
      }
    }
    return 0;
  }

  var sorted_metrics = metrics;
  column_index = parseInt(column_index);
  switch (column_index) {
    case 0:
      sorted_metrics.sort(function(a, b) {
        return (compare(
          a[column_index].toLowerCase(),
          b[column_index].toLowerCase()));
      });
      break;
    case 2:
      sorted_metrics.sort(function(a, b) {
        return (compare(
          a[column_index].toLowerCase(), 
          b[column_index].toLowerCase()));
      });
      break;
    case 3:
      sorted_metrics.sort(function(a, b) {
        return (compare(
          a[column_index], b[column_index]));
      });
      break;
    default:
      break;
  }
}

FnordMetric.util.getHorizontalEditorHeight = function(
  editor_height, result_height) {
    var default_height = (window.innerHeight - 49);
    editor_height = Math.max(editor_height, default_height);
    var height = Math.max(editor_height, result_height);
    return height;
}

FnordMetric.util.getHorizontalEditorWidth = 
  function(editor_width) {
    //returns the percental editor width
    var wdn_width = window.innerWidth;
    if (editor_width > 0) {
      editor_width = wdn_width / editor_width;
    }
    var width = Math.max(35, Math.min(50, editor_width));
    return width;
}


FnordMetric.createButton = function(href, class_name, inner_HTML) {
  var button = document.createElement("a");
  button.href = "#";
  if (class_name !== undefined) {
    button.className = class_name;
  }
  if (inner_HTML !== undefined) {
    button.innerHTML = inner_HTML;
  }
  return button;
}

/**
  * returns those metric objects whose key includes search_item
  * @param metrics array of metric objects
  */
FnordMetric.util.searchMetricList = function(metrics, search_item) {
  //FIXME works but seems not to be the best solution
  var data = [];
  metrics.map(function(item) {
    if (item.key != undefined) {
      if (item.key.indexOf(search_item) > -1) {
        data.push(item);
      }
    }
  });
  return data;
}

FnordMetric.util.htmlEscape = function(str) {
  return str;
}


/* returns all words that include filter */
FnordMetric.util.filterStringArray = function(strings, filter, limit) {
  //FIXME ?
  var data = [];
  strings.map(function(string) {
    if (string.indexOf(filter) > -1 && limit > 0) {
      data.push(string);
      limit--;
    }
  });
  return data;
}

FnordMetric.util.toMilliSeconds = function(timestr) {
  var time = timestr.split(/([a-z])/);
  var conversion = {
    "s" : 1000,
    "m" : 60000,
    "h" : 3600000,
    "d" : 86400000
  }
  var seconds = time[0] * conversion[time[1]];
  return parseInt(seconds, 10);
}

FnordMetric.util.milliSecondsToTimeString = function(seconds) {
  if (seconds < 60000) {
    return (seconds / 1000) + "s";
  }
  if (seconds < 3600000) {
    return (seconds / 60000) + "m";
  }
  return (seconds / 3600000) + "h";
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
  var start_time = Math.round(params.start_time / 1000);
  var end_time = Math.round(params.end_time / 1000);
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
  } else if (view == "rollup_sum" || view == "rollup_count" || view == "rollup_mean") {
    /* adapt draw stm */
    draw_stm = "DRAW BARCHART\n  ";
    var func = (view.split("_"))[1];

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

    hasAggregation = true;
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
        "OVER TIMEWINDOW(time, " + Math.round(t_window / 1000) + ",";

      group_expr += (t_step != undefined)?
        Math.round(t_step / 1000) : Math.round(t_window / 1000);

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

FnordMetric.util.isNumKey = function(keycode) {
  return (
    (keycode >= 48 && keycode <= 57));
}

/* tab, arrow-left, arrow-right, deletekeys */
FnordMetric.util.isNavKey = function(keycode) {
  return (
    keycode == 8 ||
    keycode == 9 ||
    keycode == 37 ||
    keycode == 39 ||
    keycode == 46);
}

FnordMetric.util.validatedTimeInput = function(time_input,type, callback) {
  time_input.maxLength = "2";
  var classname = time_input.className;
  time_input.addEventListener('keypress', function(e) {
    if (e.keyCode == 13) {
      var value = parseInt(time_input.value, 10);
      if (type == "minutes") {
        if (value > 59) {
          this.className += " highlighted";
          return;
        }
      } else {
        if (value > 23) {
          this.className += " highlighted";
          return;
        }
      }
      this.className = classname;
      callback();
      return;
    }
    if (!FnordMetric.util.isNavKey(e.keyCode) &&
      !FnordMetric.util.isNumKey(e.keyCode)) {
      e.preventDefault();
    }
  },false);
}

FnordMetric.util.appendLeadingZero = function (num) {
  var num = num;
  if (typeof num == 'string') {
    return (num.length > 1)? num : "0" + num;
  }
  return (num > 9)? num : "0" + num;
}


/* returns mm/dd/yyyy hh:mm */
FnordMetric.util.getDateTimeString = function(timestamp) {
  var timestamp = timestamp == undefined?
    new Date() : new Date(parseInt(timestamp, 10));

  var month = timestamp.getMonth();
  month = FnordMetric.util.appendLeadingZero(month +1);
  var day = FnordMetric.util.appendLeadingZero(timestamp.getDate());
  var hours = FnordMetric.util.appendLeadingZero(timestamp.getHours());
  var minutes = FnordMetric.util.appendLeadingZero(timestamp.getMinutes());
  return (
    month + "/" + day + "/" +
    timestamp.getFullYear() + "  " + hours +
    ":" + minutes);
}

FnordMetric.util.makeLowerCaseUnderscore = function(string) {
  return (string.toLowerCase().replace(/\s\s|\s/g,"_"));
}

FnordMetric.util.reverseLowerCaseUnderscore = function(string) {
  var str = string[0].toUpperCase();
  for (var i = 1; i < string.length; i++) {
    if (string[i] == "_") {
      str += " " + string[i+1].toUpperCase();
      i++;
    } else {
      str += string[i];
    }
  }
  return str;
}

/* doesn't check if value is already in list */
FnordMetric.util.addToCSV = function(list, value) {
  if (list.length == 0) {
    return value;
  }
  if (value.length == 0) {
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

FnordMetric.util.renderFlyout = function(text, elem, left) {
  var flyout = document.createElement("div");
  flyout.className = "hover_tooltip";
  flyout.innerHTML = text;
  flyout.style.top = "85px";
  flyout.style.left = left + "px";
  elem.appendChild(flyout);
  return flyout;
}


FnordMetric.util.removeIfChild = function(child_n, parent_n) {
  if (typeof child_n == 'object' && child_n.parentNode == parent_n) {
    parent_n.removeChild(child_n);
  }
}




