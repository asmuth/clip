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

MetricTools.Layout = function() {
  "use strict";

  var elem = document.getElementById("metrictools");
  var resize_callback;

  this.setResizeCallback = function(callback_fn) {
    resize_callback = callback_fn;
  };

  this.renderChart = renderChart;

  this.render = function(opts) {
    var html =
      "<div class='container'>" +
        "<div class='title_top'></div>" +
        "<div class='legend_top'></div>" +
        "<div class='chart_container'>" +
          "<div class='legend_left'></div>" +
          "<div class='chart_container_inner'>" +
            "<div class='legend_top_inner'></div>" +
            "<div class='chart'><div class='chart_canvas'><div style='width: 200px; height: 100px; background:#f0f;'>Test Content</div></div></div>" +
            "<div class='legend_bottom_inner'></div>" +
          "</div>" +
          "<div class='legend_right'></div>" +
        "</div>" +
        "<div class='title_bottom'></div>" +
        "<div class='legend_bottom'></div>" +
      "</div>";
    elem.innerHTML = html;

    renderTitle(opts.title);
    renderLegend(opts.legend);

    setChartHeight(opts.chart.height);

    if (needsResize(opts)) {
      resize(opts);
    } else {
      renderChart(opts);
    }
  };

  function resize(opts) {
    var iframe_height = window.innerHeight;
    var container_elem = elem.querySelector(".container");
    container_elem.style.height = iframe_height + "px";

    var chart_elem = elem.querySelector(".chart");
    var chart_pos = chart_elem.getBoundingClientRect();

    resize_callback(chart_pos.height);
  }

  function setChartHeight(height) {
    var chart_elem = elem.querySelector(".chart");
    chart_elem.style.height = height;

    if (height == "auto") {
      chart_elem.style.flex = "1";
    }
  }

  function needsResize(opts) {
    return !opts.chart.height || opts.chart.height == "auto";
  }

  function renderTitle(opts) {
    var title = new MetricTools.Layout.Title(elem);
    title.render(opts);
  }

  function renderLegend(opts) {
    var legend = new MetricTools.Layout.Legend(elem);
    legend.render(opts);
  }

  function renderChart(opts) {
    //TODO
  }
};

MetricTools.Layout.Title = function(elem) {
  "use strict";

  this.render = function(opts) {
    var title_elem = elem.querySelector(".title_" + opts.position);
    if (!title_elem) {
      throw "invalid title position: " + title_opts.position;
    }

    if (opts.height != "auto") {
      title_elem.style.height = opts.height;
    }

    title_elem.innerHTML = MetricTools.DOMUtil.escapeHTML(opts.title);
  };
};

MetricTools.Layout.Legend = function(elem) {
  "use strict";

  this.render = function(opts) {
    /* render legend layout */
    var legend_elem = elem.querySelector(".legend_" + opts.position);
    if (!legend_elem) {
      throw "invalid legend position: " + opts.position;
    }

    switch (opts.position) {
      case "left":
      case "right":
        if (opts.width != "auto") {
          legend_elem.style.width = opts.width;
        }
        break;

      default:
        if (opts.height != "auto") {
          legend_elem.style.height = opts.height;
        }
        break;
    }

    /* render legend content */
    for (var i = 0; i < opts.series.length; ++i) {
      var series = opts.series[i];
      var circle_elem = document.createElement("span");
      circle_elem.classList.add("circle", "color" + (i + 1));

      var series_elem = document.createElement("div");
      series_elem.appendChild(circle_elem);
      series_elem.innerHTML += MetricTools.DOMUtil.escapeHTML(series.name);
      legend_elem.appendChild(series_elem);
    }
  };
};

function fetch(url, callback_fn) {
  var fetch_url = MetricTools.URLUtil.addOrModifyURLParam(url, "format", "json");
  MetricTools.HTTP.get(fetch_url, {}, function(r) {
    try {
      var result = JSON.parse(r.response);
      callback_fn(result);
    } catch (e) {
      throw e; //FIXME handle error
    }
  });
}

function render() {
  var layout;
  var url = location.href;

  fetch(url, function(result) {
    layout = new MetricTools.Layout();

    /* fetch the chart with the right height again */
    layout.setResizeCallback(function(height) {
      url = MetricTools.URLUtil.addOrModifyURLParam(url, "height", height);
      fetch(url, layout.renderChart);
    });

    layout.render(result);
  });
}

render();

