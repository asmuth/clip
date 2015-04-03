if (typeof FnordMetric == "undefined") {
  FnordMetric = {};
}

FnordMetric.ChartExtensions = function(elem) {
  var base_elem = elem;
  var hover_points = [];
  var tooltip_elem = null;
  var bbox = null;
  var legend_elems = base_elem.querySelectorAll(".legend .point");
  var hidden_series = [];
  var chart_elems = [];
  base_elem.className.baseVal += " extended";

  var compareBBox = function(a, b) {
    if (a == null || b == null) {
      return false;
    }

    return (
        a.left == b.left &&
        a.top == b.top &&
        a.width == b.width &&
        a.height == b.height);
  }

  var indexPoints  = function(elems) {
    for (var j = 0; j < elems.length; j++) {
      var points = elems[j].querySelectorAll(".point");
      for (var i = 0; i < points.length; i++) {
        var bbox = points[i].getBoundingClientRect();
        hover_points.push({
          x: bbox.left + bbox.width * 0.5,
          y: window.scrollY + bbox.top + bbox.height * 0.5,
          top: window.scrollY + bbox.top,
          label: points[i].getAttribute('fm:label')
        });
      }
    }
  }

  var indexBarPoints = function(elems) {
    for (var i = 0; i < elems.length; i++) {
      var bbox = elems[i].getBoundingClientRect();

      hover_points.push({
        x: bbox.left + bbox.width * 0.5,
        y: window.scrollY + bbox.top + bbox.height * 0.5,
        top: window.scrollY + bbox.top,
        bbox: bbox,
        label: elems[i].getAttribute('fm:label')
      });
    }

  }

  var indexAllPoints = function() {
    hover_points = [];
    indexPoints(elem.querySelectorAll(".areas"));
    indexPoints(elem.querySelectorAll(".lines"));
    indexPoints(elem.querySelectorAll(".points"));
    indexBarPoints(elem.querySelectorAll(".bar"))
  }

  var showToolTip = function (point) {

    if (tooltip_elem == null) {
      /* setup tooltip elem */
      tooltip_elem = document.createElement("div");
      tooltip_elem.style.position = "absolute";
      tooltip_elem.style.display = "none";
      document.body.appendChild(tooltip_elem);
      tooltip_elem.className = 'fm-tooltip';
      tooltip_elem.onmousemove = chartHover;
    }

    tooltip_elem.innerHTML = point.label;
    tooltip_elem.style.display = "block";

    var pos_x = Math.round(point.x - tooltip_elem.offsetWidth * 0.5);
    tooltip_elem.style.left = pos_x + "px";

    var pos_y = Math.round(point.top - tooltip_elem.offsetHeight )-5;
    tooltip_elem.style.top = pos_y + "px";

  };

  var hideToolTip = function () {
    if (tooltip_elem != null) {
      tooltip_elem.style.display = "none";
    }
  };

  var findClosestPoint = function(x, y, max_snap) {
    if (typeof max_snap == "undefined") {
      max_snap = Infinity;
    }

    var best_point = null;
    var best_distance = max_snap;

    for (var i = 0; i < hover_points.length; i++) {
      if (hover_points[i].bbox) {
        if (
          (x >= hover_points[i].bbox.left && 
          x <= hover_points[i].bbox.right) && (
          y >= hover_points[i].bbox.top &&
          y <= hover_points[i].bbox.bottom)
          ) {
          best_point = hover_points[i];
        }
      } else {
        /* calculate the euclidian distance */
        var diff_x = Math.pow((x - hover_points[i].x), 2);
        var diff_y = Math.pow((y - hover_points[i].y), 2);
        var dist = Math.sqrt(diff_x + diff_y);

        if (dist < best_distance) { 
          best_distance = dist;
          best_point = hover_points[i];
        }
      }
    }

    return best_point;
  };

  var chartHover = function(e) {
    var mx = e.x + window.scrollX;
    var my = e.y + window.scrollY;

    if (!compareBBox(bbox, base_elem.getBoundingClientRect())) {
      indexAllPoints();
      bbox = base_elem.getBoundingClientRect();
    }

    var point = findClosestPoint(mx, my, 50);
    if (point == null) {
      hideToolTip();
    } else {
      showToolTip(point);
    }
  };

  var initChartElems = function() {
    chart_elems = base_elem.querySelectorAll(".lines circle");
    Array.prototype.push.apply(
      chart_elems, base_elem.querySelectorAll(".lines path"));
    Array.prototype.push.apply(
      chart_elems, base_elem.querySelectorAll(".points circle"));
    Array.prototype.push.apply(
      chart_elems, base_elem.querySelectorAll(".bars rect"));
    Array.prototype.push.apply(
      chart_elems, base_elem.querySelectorAll(".areas circle"));
  };

  var hideSeries = function(series) {
    for (var i = 0; i < chart_elems.length; i++) {
      if (chart_elems[i].getAttribute('fm:series') == series) {
        chart_elems[i].style.display = "none";
      }
    }
  };

  var displaySeries = function(series) {
    for (var i = 0; i < chart_elems.length; i++) {
      if (chart_elems[i].getAttribute('fm:series') == series) {
        chart_elems[i].style.display = "block";
      }
    }
  };

  var legendClick = function(legend_elem) {
    if (chart_elems.length == 0) {
      initChartElems();
    }
    var series = legend_elem.getAttribute('fm:series');
    //FIXME: add fm:series attribute to legend_elems and path_elems
    var series = 'Tokyo'; 
    var index = hidden_series.indexOf(series);
    if (index > -1) {
      displaySeries(series);
      hidden_series.splice(index, 1);
    } else {
      hidden_series.push(series);
      hideSeries(series);
    } 
  };

  base_elem.onmouseover = chartHover;
  base_elem.onmousemove = chartHover;


  for (var i = 0; i < legend_elems.length; i++) {
    legend_elems[i].onclick = function() {
      legendClick(this);
    };
  }
}

FnordMetric.ChartComponent = function(elem) {
  var query = elem.innerHTML;
  elem.innerHTML = "";
  elem.style.display = "block";
  elem.className += " loading extended";

  FnordMetric.httpPost("http://localhost:8080/query", query, function(resp) {
    elem.className = elem.className.replace("loading", "");

    if (resp.status != 200) {
      elem.innerHTML = "Server Error";
      elem.className += "error";
      return;
    }

    var resp_json = JSON.parse(resp.response);
    if (resp_json.status != "success") {
      elem.innerHTML = "error: " + resp_json.error;
      elem.className += "error";
      return;
    }

    if (typeof resp_json.charts == "undefined" || resp_json.charts.length < 1) {
      elem.innerHTML = "error: query did not return any charts";
      elem.className += "error";
      return;
    }

    elem.innerHTML = resp_json.charts[0].svg;
    FnordMetric.extendCharts();
  });
}

FnordMetric.httpGet = function(url, callback) {
  var http = new XMLHttpRequest();
  http.open("GET", url, true);
  http.send();

  http.onreadystatechange = function() {
    if (http.readyState == 4) {
      callback(http);
    }
  }
}

FnordMetric.httpPost = function(url, request, callback) {
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
}

FnordMetric.extendCharts = function() {
  var elems = document.getElementsByClassName("fm-chart");

  for (var i = 0; i < elems.length; ++i) {
    if (elems[i].className.baseVal.indexOf("extended") == -1) {
      FnordMetric.ChartExtensions(elems[i]);
    }
  }
}

FnordMetric.extendComponents = function () {
  var chart_elems = document.getElementsByTagName("fm-chart");

  for (var i = 0; i < chart_elems.length; ++i) {
    if (chart_elems[i].className.indexOf("extended") == -1) {
      FnordMetric.ChartComponent(chart_elems[i]);
    }
  }
}

document.addEventListener("DOMContentLoaded", function() {
  FnordMetric.extendComponents();
  FnordMetric.extendCharts();
}, false);
