
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

  var chart_renderer;
  var summary_renderer;

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
        var scaled_values = scaleValues(s.values, min, max);

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

    watchHoverEvent(chart_elem);
  }



  function scaleValues(values, min, max) {
    var scaled = [];

    for (var i = 0; i < values.length; ++i) {
      var v = values[i];
      var x  = i / (values.length - 1);

      var point = {
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

  function watchHoverEvent(chart_elem) {
    var circles = chart_elem.querySelectorAll("circle.point");

    for (var i = 0; i < circles.length; i++) {
      circles[i].addEventListener("mouseover", function(e) {
        console.log("display tooltio");
      }, false);

      circles[i].addEventListener("mouseout", function(e) {
        console.log("hide tooltio");
      }, false);
    }

  }

  /** init **/
  chart_renderer = new FnordMetric.SeriesChartRenderer(
    grid_margin_x,
    grid_margin_y,
    tick_margin);

  summary_renderer = new FnordMetric.SeriesChartSummaryRenderer(default_colors);

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

        circles.push("<circle class='point' r='5' cx='", dx, "' cy='", dy,
          "' fm-label='", points[i].value, "'></circle>");

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
}

