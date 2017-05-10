var MetricTools = this.MetricTools || {};

MetricTools.URLUtil = MetricTools.URLUtil || {};
MetricTools.URLUtil.addOrModifyURLParam = function(url, param, value) {
  var a = document.createElement('a');
  a.href = url;

  var vars = a.search.substr(1).split("&");
  var new_vars = "";
  var found = false;

  for (var i = 0; i < vars.length; i++) {
    if (vars[i].length == 0) {
      continue;
    }

    var pair = vars[i].split('=', 2);
    var val = pair[1];
    if (pair[0] == param) {
      val = value;
      found = true;
    }

    new_vars += pair[0] + "=" + val + "&";
  }

  if (!found) {
    new_vars += param + "=" + value;
  }

  a.search = "?" + new_vars;
  return a.pathname + a.search;
};

MetricTools.HTTP = MetricTools.HTTP || {};
MetricTools.HTTP.get = function(url, headers, callback) {
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

function renderLayout(opts) {
  var mt_elem = document.getElementById("metrictools");

  var chart_elem = document.createElement("div");
  chart_elem.innerHTML = opts.svg;
  mt_elem.appendChild(chart_elem);
};

function fetch(url) {
  var fetch_url = MetricTools.URLUtil.addOrModifyURLParam(url, "format", "json");
  MetricTools.HTTP.get(fetch_url, {}, function(r) {
    var result = JSON.parse(r.response);
    renderLayout(result);
  });
}

fetch(location.href);
