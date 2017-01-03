
/**
  {
    summary: {
      series_id: "cur_value"
    }
    time: [1...., 1....,]
    series: [
      {
        series_id: "cur_value",
        values: [2, 6, 7, ...],
        color: "#999",
        title: "Current Value"
        unit: "%"
      }
    ]
  }


**/

if (typeof FnordMetric == undefined) {
  FnordMetric = {};
}

FnordMetric.SeriesChart = function(elem, config) {
  'use strict';

  var summary_elem_width = config.hasOwnProperty("summary") ? 225 : 0;
  var grid_margin_x = 20;
  var grid_margin_y = 10;
  var tick_margin = 6;

  var default_colors = ["#19A2E5", "#aad4e9"];

  var summary_renderer;
  var chart_renderer;
  var chart_hover_handler;

  function render() {
    if (config.hasOwnProperty("summary") &&
        config.summary.hasOwnProperty("series_id")) {
      renderSummaries();
    }

    renderChart();
  }

  function renderSummaries() {
    var summary_elem = document.createElement("div");
    summary_elem.className = "summary";

    var html = summary_renderer.render(config);
    summary_elem.innerHTML = html;
    elem.appendChild(summary_elem);
  }

  function renderChart() {
    var chart_elem = document.createElement("div");
    chart_elem.className = "chart";

    var height = elem.offsetHeight;
    var width = elem.offsetWidth - summary_elem_width;

    var grid_height = height - grid_margin_x;
    var grid_width = width - grid_margin_y;

    var html = ["<svg class='fm-chart' viewBox='0 0 ", width, " ", height, "'>"];

    /** render x axis **/
    html.push(chart_renderer.renderXAxis(chart_elem, height, width));

    html.push("<g class='lines'>");

    //FIXME check for each series if unit is the same or if multiple y axis have to be rendered
    for (var i = 0; i < config.series.length; i++) {
      var s = config.series[i];

      if (s.values) {
        var min = s.min ? s.min : 0;
        var max = s.max ? s.max : Math.max.apply(null, s.values);
        var scaled_values = scaleValues(s.time, s.values, min, max);

        var color = s.color ? s.color : default_colors[i % default_colors.length];

        html.push(
          chart_renderer.renderPath(scaled_values, grid_height, width, color));
      }
    }

    html.push("</g>");

    html.push(chart_renderer.renderYAxis(chart_elem, height, width));

    html.push("</svg>");

    chart_elem.innerHTML = html.join("");
    elem.appendChild(chart_elem);

    chart_hover_handler.watch(chart_elem);
  }

  function scaleValues(time, values, min, max) {
    var scaled = [];

    for (var i = 0; i < values.length; ++i) {
      var v = values[i];
      var x  = i / (values.length - 1);

      var point = {
        time: time[i],
        value: v,
        x: x
      }

      if (v < min) {
        point.y = 0;

      } else if (v > max) {
        point.y = 1.0;

      } else if (max - min == 0) {
        point.y = 0;

      } else {
        point.y = (v - min) / (max - min);

      }

      scaled.push(point);
    }

    return scaled;
  }

  /** init **/
  summary_renderer = new FnordMetric.SeriesChartSummaryRenderer(default_colors);

  chart_renderer = new FnordMetric.SeriesChartRenderer(
    grid_margin_x,
    grid_margin_y,
    tick_margin);

  chart_hover_handler = new FnordMetric.SeriesChartHoverHandler();

  render();
}

FnordMetric.SeriesChartRenderer = function(
    grid_margin_x,
    grid_margin_y,
    tick_margin) {

  this.renderXAxis = function(chart_elem, height, width) {
    var tick_height = height - tick_margin;
    var grid_height = height - grid_margin_x;
    var grid_width = width - grid_margin_y;
    var html = [];

    html.push("<g class='axis x'>");

    /** render x ticks **/
    //FIXME make 70 constant
    html.push("<text x='", 0 + 60, "' y='", tick_height,
      "' class='label'>", "2017-01-02 15:32", "</text>");

    html.push("<text x='", width * 1/8 + 70, "' y='", tick_height,
      "' class='label'>", "2017-01-02 15:32", "</text>");

    html.push("<text x='", width * 2/8 + 70, "' y='", tick_height,
      "' class='label'>", "2017-01-02 15:30", "</text>");

    html.push("<text x='", width * 3/8 + 70, "' y='", tick_height,
      "' class='label'>", "2017-01-02 15:30", "</text>");

    html.push("<text x='", width * 4/8 + 70, "' y='", tick_height,
      "' class='label'>", "2017-01-02 15:30", "</text>");

    html.push("<text x='", width * 5/8 + 70, "' y='", tick_height,
      "' class='label'>", "2017-01-02 15:30", "</text>");

    html.push("<text x='", width * 6/8 + 70, "' y='", tick_height,
      "' class='label'>", "2017-01-02 15:30", "</text>");

    html.push("<text x='", width * 7/8 + 70, "' y='", tick_height,
      "' class='label'>", "2017-01-02 15:30", "</text>");

    /** render y ticks **/
    html.push("<text x='", grid_margin_y, "' y='", grid_height * 2 / 3,
      "' class='label'>", "10", "</text>");

    html.push("<text x='", grid_margin_y, "' y='", grid_height * 1 / 3,
      "' class='label'>", "10", "</text>");

    html.push("<text x='", grid_margin_y, "' y='", 0,
      "' class='label'>", "10", "</text>");


    /** render x axes **/
    html.push(
      "<line class='axis stroke main_axis' y1='", grid_height, "' y2='",
      grid_height, "' x1='0' x2='", grid_width, "'></line>");

    html.push(
      "<line class='axis stroke' y1='", grid_height * 2 / 3, "' y2='",
      grid_height * 2 / 3, "' x1='", grid_margin_y,
      "' x2='", grid_width, "'></line>");

    html.push(
      "<line class='axis stroke' y1='", grid_height * 1 / 3, "' y2='",
      grid_height * 1 / 3, "' x1='", grid_margin_y,
      "' x2='", grid_width, "'></line>");

    html.push(
      "<line class='axis stroke' y1='0' y2='0'",
      " x1='", grid_margin_y, "' x2='", grid_width, "'></line>");

    html.push("</g>");
    return html.join("");
  }

  this.renderYAxis = function(chart_elem, height, width) {
    var grid_height = height - grid_margin_x;
    var grid_width = width - grid_margin_y;
    var html = [];

    html.push("<g class='axis y'>");

    /** render y axes **/
    html.push(
      "<line class='axis stroke' y1='0' y2='", grid_height,
      "' x1='", grid_width * 1 / 5, "' x2='", grid_width * 1 / 5, "'></line>");

    html.push(
      "<line class='axis stroke' y1='0' y2='", grid_height,
      "' x1='", grid_width * 2 / 5, "' x2='", grid_width * 2 / 5, "'></line>");

    html.push(
      "<line class='axis stroke' y1='0' y2='", grid_height,
      "' x1='", grid_width * 3 / 5, "' x2='", grid_width * 3 / 5, "'></line>");

    html.push(
      "<line class='axis stroke' y1='0' y2='", grid_height,
      "' x1='", grid_width * 4 / 5, "' x2='", grid_width * 4 / 5, "'></line>");

    html.push("</g>");

    return html.join("");
  }

  this.renderPath = function(points, height, width, color) {
    var padding_x = 0;
    var padding_y = 0;

    var html = [];
    var circles = [];
    var svg_line = [];

    for (var i = 0; i < points.length; ++i) {
      if (!isNaN(points[i].y)) {
        var dx = padding_x + (points[i].x * (width - padding_x * 2));
        var dy = padding_y + ((1.0 - points[i].y) * (height - padding_y * 2));
        svg_line.push(i == 0 ? "M" : "L", dx, dy);

        console.log(points[i]);
        circles.push("<circle class='point' r='5' cx='", dx, "' cy='", dy,
          "' fm-label='", formatDate(points[i].time / 1000), ": ",
          points[i].value, "'></circle>");

      } else {
        //FIXME
      }
    }

    html.push(circles.join(""));

    html.push(
        "<path class='line' style='stroke:", color, ";' d='",
        svg_line.join(" "),
        "'></path>");

    return html.join("");
  };

  function formatDate(timestamp) {
    function appendLeadingZero(num) {
      if (num < 10) {
        return "0" + num;
      }

      return "" + num;
    }

    var d = new Date(timestamp)
    return [
        d.getFullYear(), "-",
        appendLeadingZero(d.getMonth() + 1), "-",
        appendLeadingZero(d.getDate()), " ",
        appendLeadingZero(d.getHours()), ":",
        appendLeadingZero(d.getMinutes())].join("");
  }
}

FnordMetric.SeriesChartSummaryRenderer = function(default_colors) {
  var summary_html = "<div class='total'>{{sum}} {{unit}}</div>" +
    "<div class='legend'>{{legend}}</div>" +
    "<div class='stats'>min={{min}} max={{max}} stddev={{stddev}}</div>";

  var legend_item_html = "<div class='legend_item'>" +
    "<span class='circle' style='background: {{color}}'></span>" +
    "<span>{{title}}</span></div>";

  this.render = function(config) {
    var html = summary_html;
    var legend_html = [];

    for (var i = 0; i < config.series.length; i++) {
      var series = config.series[i];

      if (series.series_id == config.summary.series_id) {
        /** add sum and stats values **/
        if (series.hasOwnProperty("summaries")) {
          series.summaries.forEach(function(summary) {
            switch (summary.summary) {
              case "sum":
                html = html.replace("{{sum}}", summary.value);
                break;

              case "min":
                html = html.replace("{{min}}", summary.value);
                break;

              case "max":
                html = html.replace("{{max}}", summary.value);
                break;

              case "stddev":
                html = html.replace("{{stddev}}", summary.value);
                break;

              default:
                break;
            }
          });
        }

        /** add unit **/
        if (series.hasOwnProperty("unit")) {
          html = html.replace("{{unit}}", series.unit);
        }
      }

      /** render legend item **/
      var item_html = legend_item_html;
      item_html = item_html.replace(
          "{{color}}",
          series.color ? series.color : default_colors[i]);
      item_html = item_html.replace(
          "{{title}}",
          series.title ? series.title : series.series_id);

      legend_html.push(item_html);
    }

    html = html.replace("{{legend}}", legend_html.join(""));

    html = html.replace(new RegExp('{{[a-z]*}}', 'g'), "&mdash;");
    return html;
  }
}

FnordMetric.SeriesChartHoverHandler = function() {
  var base_elem = null;
  var hover_points = [];
  var tooltip_elem = null;
  var tooltip_axis = null;
  var bbox = null;
  //var legend_elems = base_elem.querySelectorAll(".legend .point");
  var hidden_series = [];
  var chart_elems = [];

  this.watch = function(elem) {
    base_elem = elem;

    base_elem.addEventListener("mouseover", chartHover, false);
    base_elem.addEventListener("mousemove", chartHover, false);

    //for (var i = 0; i < legend_elems.length; i++) {
    //  legend_elems[i].onclick = function() {
    //    legendClick(this);
    //  };
    //}
  }

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


  var indexAllPoints = function() {
    hover_points = [];
    indexPoints(base_elem.querySelectorAll(".areas"));
    indexPoints(base_elem.querySelectorAll(".lines"));
    indexPoints(base_elem.querySelectorAll(".points"));
    //indexBarPoints(elem.querySelectorAll(".bar"))
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
          label: points[i].getAttribute('fm-label')
        });
      }
    }
  }

 // var indexBarPoints = function(elems) {
 //   for (var i = 0; i < elems.length; i++) {
 //     var bbox = elems[i].getBoundingClientRect();

 //     hover_points.push({
 //       x: bbox.left + bbox.width * 0.5,
 //       y: window.scrollY + bbox.top + bbox.height * 0.5,
 //       top: window.scrollY + bbox.top,
 //       bbox: bbox,
 //       label: elems[i].getAttribute('fm:label')
 //     });
 //   }

 // }

  var showToolTip = function (point) {
    if (tooltip_elem == null) {
      /* setup tooltip elem */
      tooltip_elem = document.createElement("div");
      tooltip_elem.style.position = "absolute";
      tooltip_elem.style.display = "none";
      base_elem.appendChild(tooltip_elem);
      tooltip_elem.className = 'fm-tooltip';
      tooltip_elem.addEventListener("mousemove", chartHover, false);

      tooltip_line 
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

 // var initChartElems = function() {
 //   chart_elems = base_elem.querySelectorAll(".lines circle");
 //   Array.prototype.push.apply(
 //     chart_elems, base_elem.querySelectorAll(".lines path"));
 //   Array.prototype.push.apply(
 //     chart_elems, base_elem.querySelectorAll(".points circle"));
 //   Array.prototype.push.apply(
 //     chart_elems, base_elem.querySelectorAll(".bars rect"));
 //   Array.prototype.push.apply(
 //     chart_elems, base_elem.querySelectorAll(".areas circle"));
 // };

 // var hideSeries = function(series) {
 //   for (var i = 0; i < chart_elems.length; i++) {
 //     if (chart_elems[i].getAttribute('fm:series') == series) {
 //       chart_elems[i].style.display = "none";
 //     }
 //   }
 // };

 // var displaySeries = function(series) {
 //   for (var i = 0; i < chart_elems.length; i++) {
 //     if (chart_elems[i].getAttribute('fm:series') == series) {
 //       chart_elems[i].style.display = "block";
 //     }
 //   }
 // };

 // var legendClick = function(legend_elem) {
 //   if (chart_elems.length == 0) {
 //     initChartElems();
 //   }
 //   var series = legend_elem.getAttribute('fm:series');
 //   //FIXME: add fm:series attribute to legend_elems and path_elems
 //   var series = 'Tokyo'; 
 //   var index = hidden_series.indexOf(series);
 //   if (index > -1) {
 //     displaySeries(series);
 //     hidden_series.splice(index, 1);
 //   } else {
 //     hidden_series.push(series);
 //     hideSeries(series);
 //   } 
 // };
}

