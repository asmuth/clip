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

/**
 *
 * TODOS:
 *
 * Query Playground:
 *  - "embed this query" opens up a popup with html/js/ruby snippets
 *  - prevent reload/navigation to other page (body onunload)
*  - stretch: display a [fake] loading bar
 *  - nice to have: represent current chart and table, view in url --> renderResultPane
 *
 * Metric list view:
 *  - write meaningful error messages
 *
 *  - fix menuitems
 *  - fix resize tooltip, vertical split
 *  - wrong URL: redirect and display message 
 *  - fix back and for (push and pop states)
 *
 */


if (FnordMetric === undefined) {
  FnordMetric = {};
}

if (FnordMetric.util === undefined) {
  FnordMetric.util = {};
}

FnordMetric.util.parseQueryString = function(qstr) {
  var path;
  var query_params = {};

  if (qstr.indexOf("?") >= 0) {
    path = qstr.substr(0, qstr.indexOf("?"))

    var params_str = qstr.substr(qstr.indexOf("?") + 1);
    var raw_params = params_str.split('&');

    /* set first param which defines view's view (metric, search ...) */
    var param = raw_params[0].split('=');
    query_params.innerView = decodeURIComponent(param[0]);
    query_params.innerViewValue = decodeURIComponent(param[1]);

    for (var i = 1; i < raw_params.length; i++) {
      var param = raw_params[i].split('=');
       query_params[decodeURIComponent(param[0])] =
          decodeURIComponent(param[1]);
    }

  } else {
    path = qstr;
  }
  console.log(query_params);
  return {
    "path": path,
    "query_params": query_params
  };
}

FnordMetric.util.setURLQueryString = function(hash, query_params, encode, push_state) {
  var path = "#"+hash+"?";

  path += query_params.innerView + "=";
  path += (encode)?
    encodeURIComponent(query_params.innerViewValue) :
    query_params.innerViewValue;

  delete query_params.innerView;
  delete query_params.innerViewValue;

  for (var param in query_params) {
    path += 
      "&" + param +
      "=" + query_params[param];
    console.log(param);
    console.log(query_params[param]);
  }

  console.log("path " + path);

  /*for (var i in query_params) {
    path += 
      query_params[i].name + "=";
    path += (encode) ? 
      encodeURIComponent(query_params[i].value) :
      query_params[i].value;
    path += (i == query_params.length -1) ? "" : "&";
  }*/

  if (push_state) {
    window.history.pushState({url:path}, "#", path);
  }

  window.location.hash = path;
}

//REMOVE??
FnordMetric.util.setFragmentURL = function(hash, name, value, encode, push_state) {
  var path = window.location.pathname;
  var value = value;
  if (encode == true) {
    value = encodeURIComponent(value);
  }
  var hash = 
    path + "#" + hash + "?" + name + "=" + value;
  window.location = hash;
  console.log("push state: " + push_state);
  if (push_state == true) {
    window.history.pushState({url: hash}, "#", hash);
  }
}


FnordMetric.util.convertArrayToString = function(array) {
  var string = "";
  if (array.length > 0) {
    string = array.join(", ");
  }
  return string;
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


FnordMetric.util.openPopup = function(elem, text) {
  function closePopup() {
    elem.removeChild(popup);
    loader.destroy(elem);
  }

  var loader = FnordMetric.util.Loader();
  loader.onClick(closePopup);
  loader.display(elem);
  var popup = document.createElement("div");
  popup.className = "popup";
  var close_btn = FnordMetric.createButton(
    "#", undefined, "X");
  var innerWindow = document.createElement("div");
  innerWindow.className = "inner_window";
  innerWindow.innerHTML = text;

  popup.appendChild(close_btn);
  popup.appendChild(innerWindow);
  elem.appendChild(popup);

  close_btn.onclick = function(e) {
    e.preventDefault();
    closePopup();
  }
}

FnordMetric.util.renderPageHeader = function(text, elem) {
  var header = document.createElement("h1");
  header.className = "page_header";
  header.innerHTML = text;

  elem.appendChild(header);
}

FnordMetric.util.parseTimestamp = function(timestamp) {
  if (timestamp == 0) {
    return "0";
  }

  var time_str;
  var timestamp = timestamp / 1000;
  var now = Date.now();
  var date = new Date(timestamp);

  var offset =  Math.floor(
    (now - timestamp) / 1000);
  if (offset < 60) {
    var label = (offset == 1)? " second ago" : " seconds ago";
    time_str  = offset + label;
  } else if (offset < 3600) {
    var time = Math.floor(offset / 60);
    var label = (time == 1)? " minute ago" : " minutes ago";
    time_str = time + label;
  } else if (offset < 86400) {
    var time =  Math.floor(offset / 3600);
    var label = (time == 1)? " hour ago" : " hours ago";
    time_str = time + label;
  } else {
    var time = Math.floor(offset / 86400);
    var label = (time == 1)? " day ago" : " days ago";
    time_str = time + label;
  }

  var months = ["Jan", "Feb", "Mar", "Apr", "May",
    "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"]

  var minutes = date.getMinutes();
  if (minutes < 10) {
    minutes = "0" + minutes;
  }

  var seconds = date.getSeconds();
  if (seconds < 10) {
    seconds = "0" + seconds;
  }

  time_str +=
    " - " + months[date.getMonth()] + 
    " " + date.getDate() +
    " " + date.getFullYear() +
    " " + date.getHours() +
    ":" + minutes +
    ":" + seconds

  return time_str;
}

FnordMetric.util.parseMilliTS = function(ts) {
  if (ts < 1000) {
    if (ts == 0) {
      return " less than 1 millisecond";
    } else if (ts == 1) {
      return " 1 millisecond";
    } else {
      return ts + " milliseconds";
    }
  } else if (ts < 60000) {
    ts = ts / 1000;
  return (ts + (ts == 1? " second" : " seconds"));
  } else {
    ts = ts / 60000;
    return (ts + (ts == 1? " minute" : " minutes"));
  }
}

FnordMetric.util.humanDateToMikroTS = function(date) {
  /* first version until datepicker is implemented */ 
  var ts;
  if (date == "NOW") {
    ts = Date.now();
  }
  return ts;
}


FnordMetric.util.humanCountRows = function(tables) {
  var num = 0;
  tables.map(function(table) {
    num += table.rows.length;
  });
  return (num == 1? num + " row" : num + " rows")
}


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

FnordMetric.util.searchMetricList = function(metrics, search_item) {
  //FIXME works but seems not to be the best solution
  var data = [];
  metrics.map(function(item) {
   if (item.key.indexOf(search_item) > -1) {
      data.push(item);
    }
  });
  return data;
}

FnordMetric.util.htmlEscape = function(str) {
  return str;
}


/* returns all words that includes filter */
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
    "h" : 3600000
  }
  var seconds = time[0] * conversion[time[1]];
  return parseInt(seconds, 10);
}


/*
  Inputs Object with default values
  inputs = {
    "show" : null,
    "aggregation" : {
      "time" : null,
      "step" : null
      },
    "time" : {
      "mseconds_to_end" : null,
      "end" : null
    }
    "group_by" : [],
    "columns" : [all possible group by columns]
  }
*/
FnordMetric.util.createQuery = function(inputs, metric) {
  var query = "";
  var timewindow = null;
  var where = "";

  var draw = "DRAW Linechart AXIS BOTTOM AXIS LEFT; ";
  var select = "SELECT time AS x, ";
  var from = " FROM `" + metric + "`";
  var show;
  var group_by = "";
  var hasAggr;
  var hasTimeWindow;

  if (inputs.show == "Value") {
    show = "value as y";
    hasTimeWindow = false;
    hasAggr = false;
  } else if (inputs.show == "Rollup") {
    draw = "DRAW BARCHART AXIS BOTTOM AXIS LEFT; ";
    var column = (inputs.group_by.length > 0) ?
      inputs.group_by[0] : inputs.columns[0];
    select = "SELECT "+ column + " AS x, ";
    show = "sum(value) as y";
    hasAggr = true;
    hasTimeWindow = false;
  } else {
    hasAggr = true;
    hasTimeWindow = true;
    show = ((inputs.show).toLowerCase() + "(value) as y");
  }

  query += draw + select + show + from;

  /* check for time --> where clause and add to query */
  if (inputs.time.mseconds_to_end != null && inputs.time.end != null ) {
    var start = inputs.time.end - inputs.time.mseconds_to_end;
    where = 
      " where time > FROM_TIMESTAMP(" + Math.round(start / 1000) + ")" +
      " and time < FROM_TIMESTAMP(" + Math.round(inputs.time.end / 1000) +")";
    //console.log(where);
  }
  //query += where;

  if (hasAggr) {
    var columns = (inputs.group_by.length > 0) ?
      inputs.group_by.join(", ") : inputs.columns[0];
    if (hasTimeWindow) {
      timewindow = 
        " GROUP OVER TIMEWINDOW(time, " +
        Math.round(inputs.aggregation.time / 1000) + ", " +
        Math.round(inputs.aggregation.step / 1000) + ")";

      query += timewindow;
      group_by = " BY " + columns;
    } else {
    /* GROUP BY */
      group_by = " GROUP BY " + columns;
    }
    query += group_by;
  }

  query += ";";
  console.log(query);
  return query;
}

FnordMetric.util.getMonthStr = function(index) {
  var months = [
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
    "December"];

  return months[index];
}

FnordMetric.util.isNumKey = function(keycode) {
  return (
    (keycode >= 48 && keycode <= 57) || (keycode >= 96 && keycode <= 105));
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

FnordMetric.util.validatedTimeInput = function (time_input, type) {
  var input;
  time_input.addEventListener('keydown', function(e) {
    if (FnordMetric.util.isNumKey(e.keyCode)) {
      var input = this.value;
      //TODO validate input
      return;
    }
    if (!FnordMetric.util.isNavKey(e.keyCode)) {
      e.preventDefault();
    }
  }, false);

}

FnordMetric.util.appendLeadingZero = function (num) {
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


