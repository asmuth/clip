/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

/**
 * Renders a chart and optionally a summary for the provided series
 *
 * @param elem   {HTMLElement} an HTML elem within the chart will be rendered
 * @param params {object}      the chart's configuration
 */
FnordMetricChart.Plotter = function(elem, params) {
  'use strict';

  var svg = null;

  var kNumDefaultXTicks = 8;
  var kNumDefaultYTicks = 3;

  this.render = function(series) {
    var x_domain = new FnordMetricChart.PlotterLinearDomain;
    x_domain.setNumTicks(kNumDefaultXTicks); //FIXME make this configurable

    var y_domain = new FnordMetricChart.PlotterLinearDomain; //FIXME allow multiple y_domains
    y_domain.setNumTicks(kNumDefaultYTicks); //FIXME make this configurable

    series.forEach(function(s) {
      y_domain.findMinMax(s.values);
      x_domain.findMinMax(s.time);
    });

    var html = [];
    html.push(startCanvas());

    var x_labels = x_domain.getLabels();
    html.push(drawXAxis(x_labels));

    var y_labels = y_domain.getLabels();
    html.push(drawYAxis(y_labels));

    //drawGrid

    series.forEach(function(s) {
      drawLine(s, x_domain, y_domain);
    });

    html.push(endCanvas());
    elem.innerHTML = html.join("");
  }

  function startCanvas() {
    //FIXME add viewBox viewBox='0 0 ", width, " ", height, "'
    return "<svg class='fm-chart'>"
  }

  function endCanvas() {
    return "</svg>";
  }

  function drawXAxis(labels) {
    var html = [];

    html.push("<g class='axis x'>");

    /** render ticks **/
    for (var i = 0; i < labels.length; i++) {
      html.push(
        "<text x='", labels[i][0], "' y='0' class='label'>",
          formatDate(labels[i][1] / 1000),
        "</text>");
    }

    /** render line **/
   // html.push(
   //   "<line class='axis stroke main_axis' ",
   //     "y1='", height * i / num_axes, "' y2='", height,
   //     "' x1='0' x2='", width,
   //   "'></line>");

    html.push("</g>");

    return html.join("");
  }

  function drawYAxis(labels) {
    var html = [];

    html.push("<g class='axis y'>");

    /** render ticks **/
    for (var i = 0; i < labels.length; i++) {
      html.push(
        "<text class='label' x='0' y='", labels[i][0], "'>",
          DOMUtil.escapeHTML(labels[i][1]),
        "</text>");
    }

    html.push("</g>");

    return html.join("");
  }

  function drawLine(series, x_domain, y_domain) {
    var line = [];

    for (var i = 0; i < series.values.length; i++) {
      var x = x_domain.convertDomainToScreen(series.time[i]);
      var y = y_domain.convertDomainToScreen(series.values[i]);



      console.log(x, y)
    }
  }

   function drawLines(series, points, height, width) {
    var grid_width = width - padding.left - padding.right;
    var grid_height = height - padding.top - padding.bottom
    var padding_x = 0;
    var padding_y = 0;

    var html = [];
    var circles = [];
    var svg_line = [];

    for (var i = 0; i < points.length; ++i) {
      if (!isNaN(points[i].y)) {
        var dx = padding_x + (points[i].x * (grid_width - padding_x * 2));
        var dy = padding.top + ((1.0 - points[i].y) * (grid_height));
        svg_line.push(i == 0 ? "M" : "L", dx, dy);

        circles.push(
          "<circle class='point' r='5'",
            " cx='", dx, "' cy='", dy,
            "' fm-date='", formatDate(points[i].time / 1000),
            "' fm-value='", points[i].value,
            "' fm-color='", series.color,
            "' fm-title='", series.title || series.series_id,
          "'></circle>");

      } else {
        //FIXME
      }
    }

    html.push(circles.join(""));

    html.push(
        "<path class='line' style='stroke:", series.color, ";",
          "' fm-series='", series.series_id,
          "' d='", svg_line.join(" "),
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

