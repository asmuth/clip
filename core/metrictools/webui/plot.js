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

MetricTools.DOMUtil = MetricTools.DOMUtil || {};
MetricTools.DOMUtil.escapeHTML = function(str) {
  if (!str) {
    return "";
  }

  var elem = document.createElement("div");
  var text = document.createTextNode(str);
  elem.appendChild(text);

  return elem.innerHTML;
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

function renderChart(elem, opts) {
  elem.innerHTML = opts.svg;
}

function renderLegend(elem, opts) {
  console.log(opts.legend);
  for (var i = 0; i < opts.legend.series.length; ++i) {
    var series = opts.legend.series[i];

    var circle_elem = document.createElement("span");
    circle_elem.classList.add("circle", "color" + (i + 1));

    var series_elem = document.createElement("div");
    series_elem.appendChild(circle_elem);
    series_elem.innerHTML += MetricTools.DOMUtil.escapeHTML(series.name);
    elem.appendChild(series_elem);
  }
}

function renderLayout(opts) {
  var mt_elem = document.getElementById("metrictools");

  var chart_elem = document.createElement("div");
  chart_elem.classList.add("chart");

  var legend_elem = document.createElement("div");
  legend_elem.classList.add("legend");

  mt_elem.appendChild(chart_elem);
  renderChart(chart_elem, opts);

  mt_elem.appendChild(legend_elem);
  renderLegend(legend_elem, opts);
}

function fetch(url) {
  var fetch_url = MetricTools.URLUtil.addOrModifyURLParam(url, "format", "json");
  MetricTools.HTTP.get(fetch_url, {}, function(r) {
    var result = JSON.parse(r.response);
    renderLayout(result);
  });
}

fetch(location.href);
