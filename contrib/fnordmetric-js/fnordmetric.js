if (typeof FnordMetric == "undefined") {
  FnordMetric = {};
}

FnordMetric.ChartExtensions = function(elem) {
  var base_elem = elem;
  var hover_points = [];
  var tooltip_elem = null;
  var bbox = null;
  var legend_elems = base_elem.querySelectorAll(".legend .point");

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
        x: bbox.left + bbox.width *0.5,
        y: window.scrollY + bbox.top + bbox.height * 0.5,
        top: window.scrollY + bbox.top,
        bbox: bbox,
        label: "foobar"
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

    /* calculate the euclidian distance */
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

  base_elem.onmouseover = chartHover;
  base_elem.onmousemove = chartHover;
}

FnordMetric.extendCharts = function() {
  var elems = document.getElementsByClassName("fm-chart");

  for (var i = 0; i < elems.length; ++i) {
    FnordMetric.ChartExtensions(elems[i]);
  }
}

