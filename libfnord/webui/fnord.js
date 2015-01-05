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

Fnord.ready();
