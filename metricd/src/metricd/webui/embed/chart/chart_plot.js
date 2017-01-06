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

  this.render = function(series) {
    return;
    var domain = new FnordMetricChart.PlotterLinearDomain;
    series.forEach(function(s) {
      domain.findMinMax(s);
    });

    drawXAxis()
    drawYAxis()

    series.forEach(function(s) {
      drawLine(s);
    });
  }

  function blah() {
    var chart_elem = document.createElement("div");
    chart_elem.className = "chart";

    var height = elem.offsetHeight;
    var width = elem.offsetWidth - summary_elem_width;

    var time_values = config.time;

    /** build chart html **/
    var html = [];
    html.push("<svg class='fm-chart' viewBox='0 0 ", width, " ", height, "'>");

    /** render x axis **/
    html.push(chart_renderer.renderXAxis(
        time_values[0] / 1000,
        time_values[time_values.length - 1] / 1000,
        chart_elem,
        height,
        width));

    html.push("<g class='lines'>");

    for (var i = 0; i < config.series.length; i++) {
      var s = config.series[i];
      if (!s.values) {
        return false;
      }

      var scaled_values = scaleValues(
          time_values,
          s.values,
          min,
          max);

      if (!s.color) {
        s.color = default_colors[i % default_colors.length];
      }

      html.push(chart_renderer.renderPath(s, scaled_values, height, width));
    }

    html.push("</g>");

    html.push(chart_renderer.renderYAxis(
      min,
      max,
      chart_elem,
      height,
      width));

    html.push("</svg>");

    chart_elem.innerHTML = html.join("");
    elem.appendChild(chart_elem);

  }

  function drawXAxis(tick_values, height, width) {
    var html = [];
    html.push("<g class='axis x'>");

    /** render x ticks **/
    var num_ticks = tick_values.length;
    for (var i = 0; i < num_ticks; i++) {
      html.push(
        "<text x='", grid_width * i / num_ticks, "' y='", 0, "' class='label'>",
          formatDate(tick_values[i]),
        "</text>");
    }

    /** render x axes **/
    var num_axes = 3; //FIXME
    for (var i = 0; i < num_axes; i++) {
      html.push(
        "<line class='axis stroke main_axis' ",
          "y1='", height * i / num_axes, "' y2='", height,
          "' x1='0' x2='", width,
        "'></line>");
    }

    html.push("</g>");
    return html.join("");
  }

  function drawYAxis(min, max, chart_elem, height, width) {
    var grid_height = height - padding.top - padding.bottom;
    var grid_width = width - padding.left - padding.right;
    var html = [];

    html.push("<g class='axis y'>");

    var tick_values = getTickValues(min, max, 4);

    /** render y ticks **/
    html.push(
      "<text class='label'",
          "x='", padding.left, "' y='", grid_height * 2 / 3 + padding.top, "'>",
        tick_values[1],
      "</text>");

    html.push(
      "<text class='label' ",
          "x='", padding.left, "' y='", grid_height * 1 / 3 + padding.top, "' >",
        tick_values[2],
      "</text>");

    html.push(
      "<text class='label' x='", padding.left, "' y='", padding.top, "'>",
        tick_values[3],
      "</text>");

    /** render y axes **/
    html.push(
      "<line class='axis stroke'",
        " y1='", padding.top, "' y2='", grid_height + padding.top,
        "' x1='", grid_width * 1 / 5, "' x2='", grid_width * 1 / 5,
      "'></line>");

    html.push(
      "<line class='axis stroke'",
        " y1='", padding.top, "' y2='", grid_height + padding.top,
        "' x1='", grid_width * 2 / 5, "' x2='", grid_width * 2 / 5,
      "'></line>");

    html.push(
      "<line class='axis stroke'",
        " y1='", padding.top, "' y2='", grid_height + padding.top,
        "' x1='", grid_width * 3 / 5, "' x2='", grid_width * 3 / 5,
      "'></line>");

    html.push(
      "<line class='axis stroke'",
        "y1='", padding.top, "' y2='", grid_height + padding.top,
        "' x1='", grid_width * 4 / 5, "' x2='", grid_width * 4 / 5,
      "'></line>");

    /** render tooltip line **/
    html.push(
      "<line class='stroke tooltip'",
        " y1='", padding.top, "' y2='", grid_height + padding.top,
        "' x1='0' x2='0'>",
      "</line>");

    html.push("</g>");

    return html.join("");
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

  /**
    * Calculates the tick values linearly
    * //FIXME improve by adding a log function for smoother values and better number rounding
    */
  function getTickValues(min, max, num_ticks) {
    var range = min < 0 ? Math.abs(min) + Math.abs(max) : max - min;
    var incr = range / (num_ticks - 1);
    var tick_values = [];
    for (var i = 0; i < num_ticks; i++) {
      tick_values.push(Math.round(min + i * incr));
    }

    return tick_values;
  }

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

