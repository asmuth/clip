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
Fnord = {
  components: {}
};

Fnord.getTemplate = function(component, template) {
  var template_selector = "#" + component + "-" + template + "-tpl";

  var template = document.querySelector(template_selector);
  if (!template) {
    var template_import = document.querySelector(
        "link[data-component='" + component + "']");

    if (!template_import) {
      return null;
    }

    template = template_import.import.querySelector(template_selector);
  }

  if (!template) {
    return null;
  }

  return document.importNode(template.content, true);
};

Fnord.registerComponent = function(component, cb) {
  var proto = Object.create(HTMLElement.prototype);
  cb.apply(proto);
  Fnord.components[component] = proto;
  document.registerElement(component, { prototype: proto });
  return proto;
};

Fnord.ready = function() {
  window.dispatchEvent(new Event("fn-ready"));
  document.body.removeAttribute("data-unresolved");
};

Fnord.httpGet = function(url, callback) {
  var http = new XMLHttpRequest();
  http.open("GET", url, true);
  http.send();

  var base = this;
  http.onreadystatechange = function() {
    if (http.readyState == 4) {
      callback(http);
    }
  }
};

Fnord.httpPost = function(url, request, callback) {
  var http = new XMLHttpRequest();
  http.open("POST", url, true);
  var start = (new Date()).getTime();
  http.send(request);

  http.onreadystatechange = function() {
    if (http.readyState == 4) {
      var end = (new Date()).getTime();
      var duration = end - start;
      callback(http, duration);
    }
  }
};

Fnord.jsonRPC = function(url, method, params, callback) {
  var req = {
    "jsonrpc": "2.0",
    "method": method,
    "params": params,
    "id": 0
  };

  var http = new XMLHttpRequest();
  http.open("POST", url, true);
  var start = (new Date()).getTime();
  http.send(JSON.stringify(req));

  http.onreadystatechange = function() {
    if (http.readyState == 4) {
      var end = (new Date()).getTime();
      var duration = end - start;

      if (http.status != 200) {
        console.log("RPC failed", http.responseText);
        return;
      }

      var resp = JSON.parse(http.responseText);
      if (resp.error) {
        console.log("RPC failed", resp.error);
        return;
      }

      callback(resp.result);
    }
  }
};

Fnord.setAttributes = function(attrs, elem) {
  for (var key in attrs) {
    elem.setAttribute(key, attrs[key]);
  }
}

Fnord.parseQueryString = function(qstr) {
  var path;
  var query_params = {};

  if (qstr.indexOf("?") >= 0) {
    path = qstr.substr(0, qstr.indexOf("?"));

    var params = qstr.substr(qstr.indexOf("?") + 1).split('&');
    for (var i = 0; i < params.length; ++i) {
      var param = params[i].split('=');
      query_params[decodeURIComponent(param[0])] = decodeURIComponent(param[1]);
    }
  } else {
    path = qstr;
  }

  return {
    "path": path,
    "params": query_params
  };
}

Fnord.parseUrlQueryString = function(qstr) {
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

Fnord.parseMetricQueryUrl = function(qstr) {
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

      if (Fnord.isMetricParam(key)) {
        metricCollector[key] = value;
      } else if (key == "metric") {
        //new submetric
        params["metric" + metricIndex] = metricCollector;
        metricIndex++;
        mainMetric = false;
        metricCollector = {};
        metricCollector.name = value;
      } else if (Fnord.isGenericParam(key)) {
        params[key] = value;
      }
    }

    params["metric" + metricIndex] = metricCollector;
    params.metrics = metricIndex + 1;
  }

  return params;
};

Fnord.setUrlFromQueryString = function(hash, query_params, push_state) {
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

Fnord.isMetricParam = function(key) {
  var metricParamKeys = ["aggr_fn", "aggr_window", "aggr_step", "scale", "group_by"];
  return (metricParamKeys.indexOf(key) > -1);
}

Fnord.isGenericParam = function(key) {
  //params that can be changed
  var genericParamKeys = ["from", "until", "logarithmic", "inverted", "metrics", "format"];
  return (genericParamKeys.indexOf(key) > -1);
}

Fnord.openUrl = function(url) {
  alert("openurl is not defined!");
}

Fnord.ready();
