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
  };

  function renderTitle(title_opts) {
    if (!title_opts) {
      return;
    }

    var title_elem;
    switch (title_opts.position) {
      case "top":
        title_elem = elem.querySelector(".title_top");
        break;

      case "bottom":
        title_elem = elem.querySelector(".title_bottom");
        break;

      default:
        throw "invalid title position: " + title_opts.position;
    }

    switch (title_opts.height) {
      case "auto":
        break;

      default:
        title_elem.style.height = title_opts.height;
        break;
    }

    title_elem.innerHTML = MetricTools.DOMUtil.escapeHTML(title_opts.title);
  }
}

function fetch(url) {
  var fetch_url = MetricTools.URLUtil.addOrModifyURLParam(
      url,
      "width",
      window.innerWidth);
  fetch_url = MetricTools.URLUtil.addOrModifyURLParam(url, "format", "json");
  MetricTools.HTTP.get(fetch_url, {}, function(r) {
    try {
      var result = JSON.parse(r.response);
      var layout = new MetricTools.Layout;
      layout.render(result);
    } catch (e) {
      throw e; //FIXME handle error
    }

  });
}

fetch(location.href);
