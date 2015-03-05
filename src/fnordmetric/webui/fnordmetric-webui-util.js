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
FnordMetric.util.parseUrlQueryString = function(qstr) {
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
  }
};

FnordMetric.util.buildUrlQueryStr = function(query_params) {
  var qstr = "";

  if ("innerView" in query_params && query_params.innerView != undefined) {
    qstr += encodeURIComponent(query_params.innerView) + "=";
    qstr += encodeURIComponent(query_params.innerViewValue);

    for (var param in query_params) {
      if (param != "innerView" && 
          param != "innerViewValue" &&
          query_params[param] != undefined &&
          query_params[param].length > 0) {

        qstr += 
          "&" + encodeURIComponent(param) +
          "=" + encodeURIComponent(query_params[param]);
      }
    }
  }

  return qstr;
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
  var path = "#" + hash + "?" + FnordMetric.util.buildUrlQueryStr(query_params);

  if (push_state) {
    window.history.pushState({url:path}, "#", path);
  }
  window.location.hash = path;
  return path;
}

FnordMetric.util.isMetricParam = function(key) {
  var metricParamKeys = ["aggr_fn", "aggr_window", "aggr_step", "scale", "group_by"];
  return (metricParamKeys.indexOf(key) > -1);
}

FnordMetric.util.isGenericParam = function(key) {
  //params that can be changed
  var genericParamKeys = ["from", "until", "logarithmic", "inverted", "metrics", "format"];
  return (genericParamKeys.indexOf(key) > -1);
}


FnordMetric.util.openUrl = function(url) {
  alert("openurl is not defined!");
}

FnordMetric.util.setUrlHash = function(url_hash) {
  window.history.pushState({url: url_hash}, "#", url_hash);
  window.location.hash = url_hash;
}

FnordMetric.util.parseMetricQueryUrl = function(qstr) {
  if (qstr == null) {return;}
  var path;
  var params = {};
  var metricCollector = {};
  var metricIndex = 0;

  if (qstr.indexOf("?") >= 0) {
    path = qstr.substr(0, qstr.indexOf("?"));
    path = path.replace("#", "");

    var params_str = qstr.substr(qstr.indexOf("?") + 1);
    var raw_params = params_str.split('&');

    /*set main metric */
    var metric = raw_params[0].split('=');
    if (metric[0] != "metric") {return;}

    metricCollector.name = decodeURIComponent(metric[1]);

    for (var i = 1; i < raw_params.length; i++) {
      var param = raw_params[i].split("=");
      if (!param[0]) {continue;}

      var key = decodeURIComponent(param[0]);
      var value = decodeURIComponent(param[1]);

      if (FnordMetric.util.isMetricParam(key)) {
        metricCollector[key] = value;
      } else if (key == "metric") {
        //new submetric
        params["metric" + metricIndex] = metricCollector;
        metricIndex++;
        mainMetric = false;
        metricCollector = {};
        metricCollector.name = value;
      } else if (FnordMetric.util.isGenericParam(key)) {
        params[key] = value;
      }
    }

    params["metric" + metricIndex] = metricCollector;
    params.metrics = metricIndex + 1;
  }

  return params;
};


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

FnordMetric.util.openMetricPreview = function(metric_key) {
  var end = Math.round(Date.now() / 1000);
  var start = end - 3600;
  var raw_url =
    "metric?metric="+ encodeURIComponent(metric_key) +
    "&aggr_fn=sum&&aggr_window=5&aggr_step=10&from=" + start +
    "&until=" + end +
    "&scale=1.0";

  window.location.hash = raw_url;
  location.reload();
};

FnordMetric.util.initAppbar = function(appbar, onRefresh, getUrl) {
  var embed_btn = appbar.querySelector("fn-button[name='embed']");
  var autorefresh_btn = appbar.querySelector("fn-button[name='auto_refresh']");
  var modal = document.querySelector("fn-modal[name='embed']");

  var src_elems = modal.querySelectorAll("[name='src']");
  var location_elems = modal.querySelectorAll("[name='location']");
  var origin = document.location.origin;

  var interval_id;
  var base = this;

  appbar.classList.remove("hidden");
  embed_btn.classList.remove("hidden");
  embed_btn.classList.remove("hidden");
  appbar.querySelector("fn-search").classList.add("hidden");

  appbar.querySelector("fn-tooltip#ttp_embed").init(embed_btn);
  appbar.querySelector("fn-tooltip#ttp_refresh").init(autorefresh_btn);

  autorefresh_btn.addEventListener('fn-button-click', function() {
    if (this.getAttribute('data-state') == "active") {
      window.clearInterval(interval_id);
      this.removeAttribute('data-state');
    } else {
      interval_id = window.setInterval(function() {onRefresh(); }, 30000);
      this.setAttribute('data-state', 'active');
    }
  }, false);

  for (var i = 0; i < location_elems.length; i++) {
    location_elems[i].innerHTML = origin;
  }

  embed_btn.addEventListener('fn-button-click', function() {
    var url =  getUrl();

    for (var i = 0; i < src_elems.length; i++) {
      src_elems[i].innerHTML = origin + "/metrics/timeseries?" + url;
    }

    modal.show();
  }, false);
}



