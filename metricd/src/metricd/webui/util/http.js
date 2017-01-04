/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
HTTPUtil = this.HTTPUtil || {};

HTTPUtil.httpGet = function(url, headers, callback) {
  var http = new XMLHttpRequest();
  http.open("GET", url, true);
  for (var k in headers) {
    http.setRequestHeader(k, headers[k]);
  }

  var start = (new Date()).getTime();
  http.send();

  var base = this;
  http.onreadystatechange = function() {
    if (http.readyState == 4) {
      var end = (new Date()).getTime();
      http.runtime = end - start;
      callback(http);
    }
  }
};

HTTPUtil.httpPost = function(url, body, headers, callback) {
  var http = new XMLHttpRequest();
  http.open("POST", url, true);
  for (var k in headers) {
    http.setRequestHeader(k, headers[k]);
  }

  var start = (new Date()).getTime();
  http.send(body);

  http.onreadystatechange = function() {
    if (http.readyState == 4) {
      var end = (new Date()).getTime();
      http.runtime = end - start;
      callback(http);
    }
  }
};

HTTPUtil.buildQueryString = function(params) {
  var qs = "";

  for (var key in params) {
    var value = params[key];
    qs += encodeURIComponent(key) + "=" + encodeURIComponent(value) + "&";
  }

  return qs;
}

